/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui.h"
#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/Control.h"
#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SimpleLayout.h"
#include "openocl/ui/Textual.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;

Container::Container()
{
	this->setEdge( 0 );
	this->setMargin( 0 );
	this->setBorder( 0 );
	this->setPadding( 0 );
	this->components = new Sequence();
	this->layoutManager = new SimpleLayout();
	this->deleteableLayoutManager = this->layoutManager;
	this->background = null;
	this->redrawAll = true;
}

Container::Container( const String& name ) : Component( name )
{
	this->setEdge( 0 );
	this->setMargin( 0 );
	this->setBorder( 0 );
	this->setPadding( 0 );
	this->components = new Sequence();
	this->layoutManager = new SimpleLayout();
	this->deleteableLayoutManager = this->layoutManager;
	this->background = null;
	this->redrawAll = true;
}

Container::~Container()
{
	Iterator* it = this->components->iterator();
	while ( it->hasNext() )
	{
		try
		{
			Object& obj = const_cast<Object&>( it->next() );
			Component* com = dynamic_cast<Component*>( &obj );
			com->removeEventListener( *this );
		} catch ( Exception* ex ) {
			delete ex;
		} catch ( ... ) {}
	}
	delete it;

	delete this->components;
	delete this->deleteableLayoutManager;
	delete this->background;
}

//------------------------------------------------------------------------
//	public virtual methods (EventListener)
//------------------------------------------------------------------------

void
Container::deliverEvent( const Event& anEvent )
{
	unsigned int type = anEvent.getType();

	if ( Event::CHANGE_EVENT == type )
	{
		//fprintf( stderr, "%s.Container::deliverEvent( CHANGE_EVENT )\n", this->getName().getChars() );
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		if ( ChangeEvent::RELAYOUT == ce.getValue() )
		{
			//	As an optimization when a relayout is initiated, if the bounds are the same as before
			//	it is assumed that a component does not need to be relayed out. However, if a component
			//	does need to be relayed out because it has changed, it can fire a ChangeEvent with
			//	ChangeEvent::RELAYOUT as the type. Setting boundsChanged to true here will enforce the
			//	relayout.

			//fprintf( stderr, "\t%s.Container::deliverEvent( RELAYOUT )\n", this->getName().getChars() );
			this->setBoundsChanged( true );
		}
		this->setChanged( true );
		this->fireEvent( anEvent );
	}
	else if ( Event::ACTION_EVENT == type )
	{
		//fprintf( stderr, "%s.Container::deliverEvent( ACTION_EVENT )\n", this->getName().getChars() );
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		if ( ae.getCommand().startsWith( "REFRESH.openocl::ui::Panel" ) )
		{
			this->fireRefreshEvent( anEvent );
		}
		else
		{
			this->handleActionEvent( ae );
			this->fireEvent( anEvent );
		}
	}
	else
	{
		this->fireEvent( anEvent );
	}
}

//------------------------------------------------------------------------
//	public virtual methods (Component)
//------------------------------------------------------------------------

void
Container::setBounds( int x, int y, unsigned int width, unsigned int height )
{
	#ifdef DEBUG_OPENOCL_UI_CONTAINER_SETBOUNDS
	fprintf( stderr, "%s.Container::setBounds( %i:%i %u-%u\n", this->getName().getChars(), x, y, width, height );
	#endif
	this->Component::setBounds( x, y, width, height );
	if ( this->haveBoundsChanged() )
	{
		//	As the bounds of this container have changed we need to set the container
		//	to redrawAll so that it will repaint the background. We also need to send
		//	a change event to all children so that they will be redrawn also.

		#ifdef DEBUG_OPENOCL_UI_CONTAINER_SETBOUNDS
		fprintf( stderr, "Container::setBounds: %s bounds changed, redrawing all and forwarding change event\n", this->getName().getChars() );
		#endif

		ChangeEvent ce( *this );
		this->forwardEvent( ce );
		this->setRedrawAll( true );
	}
}

void
Container::forwardEvent( const Event& anEvent )
{
	flags type = anEvent.getType();

	bool change_event = (Event::CHANGE_EVENT == (Event::CHANGE_EVENT & type ));

	//if ( 0 != (type & this->eventMask) )

	if ( change_event || (this->eventMask == (this->eventMask | type )) )
	{
		//	if ( Event::POINTER_EVENT == (Event::POINTER_EVENT & anEvent.getType()) )
		//	{
		//		const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
		//			//  	fprintf( stderr, "Container::forwardEvent: %3i:%3i\n", pe.getX(), pe.getY() );
		//	}

		bool pointer_event = (Event::POINTER_EVENT == (Event::POINTER_EVENT & type));
		bool mouse_event = (Event::MOUSE_EVENT == (Event::MOUSE_EVENT & type));

		MouseEvent* neutered = null;
		int   pe_x;
		int   pe_y;

		if ( change_event )
		{
			const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
			this->setChanged( true );
			unsigned int value = ce.getValue();
			if ( ChangeEvent::REDRAW_ALL == value )
			{
				//fprintf( stderr, "%s.Container::forwardEvent: redraw all\n", this->getName().getChars() );
				this->setRedrawAll( true );
			}
			else if ( ChangeEvent::REDRAW_PARTIAL == value )
			{
				//	Should really check to see whether this container
				//	is within the exposed area, but due to redraw would
				//	need to redraw anyway.

				//fprintf( stderr, "%s.Container::forwardEvent: redraw partial\n", this->getName().getChars() );
				this->setRedrawAll( true );
			}
		}			
		else
		{
			this->processEvent( anEvent );
			if ( mouse_event )
			{
				const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
				pe_x = me.getX();
				pe_y = me.getY();
				neutered = new MouseEvent( me.getEventSource(), me.getTargetId(), pe_x, pe_y, 0, 0, 0, 0 );
			}
			else if ( pointer_event )
			{
				const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
				pe_x = pe.getX();
				pe_y = pe.getY();
			}
		}

		Component::eventsForwarded++;

		Region bounds;
		Sequence& components = this->getComponents();
		unsigned int max = components.getLength();
		for ( unsigned int i=0; i < max; i++ )
		{
			Object& obj = components.elementAt( i );
			Component& com = dynamic_cast<Component&>( obj );
			
			if ( mouse_event )
			{
				bounds = com.getBounds();
				if ( bounds.contains( pe_x, pe_y ) )
				{
					com.forwardEvent( anEvent );
				} else {
					com.forwardEvent( *neutered );
				}
			} else {
				com.forwardEvent( anEvent );
			}
		}

		delete neutered;
	}
}

void
Container::doLayout( const GraphicsContext& gc )
{
	//IO::err().printf( "Container::doLayout()\n" );
	/**
	 *  This lays out subcomponents based on the current
	 *  bounds of the Container.
	 */

	bool bounds_changed = this->haveBoundsChanged();
	bool changed = this->hasChanged();

	if ( bounds_changed || changed )
	{
		//fprintf( stderr, "Container::doLayout: (1)\n" );
		this->layoutManager->doLayout( *this, gc );
	}
	this->Component::setBoundsChanged( false );
}

void
Container::setContainingPanel( Panel& aPanel )
{
#ifdef DEBUG_OPENOCL_UI_CONTAINER
	fprintf( stderr, "%s.Container::setContainingPanel()\n", this->getName().getChars() );
#endif

	this->Component::setContainingPanel( aPanel );
	Iterator* it = this->components->iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		com.setContainingPanel( aPanel );
	}
	delete it;
}

void
Container::resetContainingPanel()
{
#ifdef DEBUG_OPENOCL_UI_CONTAINER
	fprintf( stderr, "%s.Container::resetContainingPanel()\n", this->getName().getChars() );
#endif

	this->Component::resetContainingPanel();
	Iterator* it = this->components->iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		com.resetContainingPanel();
	}
	delete it;
}

void
Container::setName( const String& aName )
{
	this->Component::setName( aName );
} 

//------------------------------------------------------------------------
//	public methods
//------------------------------------------------------------------------

void
Container::handleActionEvent( const ActionEvent& ae )
{}

void
Container::add( Component* aComponent )
{
	Panel* panel = dynamic_cast<Panel*>( this );
	if ( panel )
	{
		aComponent->setContainingPanel( *panel );
	}
	else if ( this->hasContainingPanel() )
	{
		aComponent->setContainingPanel( this->getContainingPanel() );
	}
	aComponent->addEventListener( *this );
	this->components->add( aComponent );
}

void
Container::add( Component& aComponent )
{
	Panel* panel = dynamic_cast<Panel*>( this );
	if ( panel )
	{
		aComponent.setContainingPanel( *panel );
	}
	else if ( this->hasContainingPanel() )
	{
		aComponent.setContainingPanel( this->getContainingPanel() );
	}
	aComponent.addEventListener( *this );
	this->components->add( aComponent );
}

void
Container::remove( Component& aComponent )
{
	Component* ref = &aComponent;
	unsigned int max = this->components->getLength();
	unsigned int i;
	for ( i=0; i < max; i++ )
	{
		Object& obj = const_cast<Object&>( this->components->elementAt( i ) );
		Component* com = dynamic_cast<Component*>( &obj );
			
		if ( com == ref )
		{
			this->components->remove( i );
			com->removeEventListener( *this );
			com->resetContainingPanel();
			break;
		}
	}

	max = this->components->getLength();
	for ( i=0; i < max; i++ )
	{
		Object& obj = const_cast<Object&>( this->components->elementAt( i ) );
		Component* com = dynamic_cast<Component*>( &obj );
			
		if ( com == ref )
		{
			fprintf( stderr, "Container::remove: removing duplicate component, aborting!\n" );
			abort();
		}
	}
}

void
Container::removeAll()
{
	unsigned int max = this->components->getLength();

	for ( unsigned int i=0; i < max; i++ )
	{
		Object& obj = const_cast<Object&>( this->components->elementAt( i ) );
		Component* com = dynamic_cast<Component*>( &obj );
		com->removeEventListener( *this );
	}
	this->components->removeAllElements();
	delete this->components;
	this->components = new Sequence();
}

void
Container::setLayout( LayoutManager* aLayoutManager )
{
	delete this->deleteableLayoutManager;
	this->layoutManager = aLayoutManager;
	this->deleteableLayoutManager = aLayoutManager;
}

void
Container::setLayout( LayoutManager& aLayoutManager )
{
	delete this->deleteableLayoutManager;
	this->layoutManager = &aLayoutManager;
	this->deleteableLayoutManager = null;
}

void
Container::setBackground( const openocl::imaging::Color& aColor )
{
	delete this->background;
	this->background = new Color( aColor );
}

void
Container::resetBackground()
{
	delete this->background;
	this->background = null;
}

//------------------------------------------------------------------------
//	public virutal methods
//------------------------------------------------------------------------

Sequence&
Container::getComponents()
{
	return *this->components;
}

//------------------------------------------------------------------------
//	public pseudo-constant methods
//------------------------------------------------------------------------

void
Container::setRedrawAll( bool value ) const
{	
	const_cast<Container*>( this )->redrawAll = value;
}

//------------------------------------------------------------------------
//	public virtual const methods (Component)
//------------------------------------------------------------------------

bool
Container::draw( GraphicsContext& gc ) const
{
	//	If you experience problems with parts of your panel not clearing
	//	you will probably look here as you expect that this is where it
	//	should be cleared.
	//	The panel is only cleared if a background color is explicitly specified
	//	using the Container::setBackground( Color& ) method.

	//fprintf( stderr, "%s.Container::draw: (1)\n", this->getName().getChars() );

	bool redraw_all = this->isRedrawAll();
	bool changed = this->hasChanged();
	bool draw_again = false;

	//	Draws each component of the container to
	//	the buffer.
	//
	//	As each element

	if ( changed )
	{
		//fprintf( stderr, "%s.Container::draw: (2)\n", this->getName().getChars() );
		Region bounds = this->getBounds();

		if ( this->background )
		{
			gc.setForeground( *this->background );
			gc.setBackground( *this->background );
			gc.setFill( *this->background );
		}

		if ( redraw_all && this->background )
		{
			//fprintf( stderr, "%s.Container::draw: (3a)\n", this->getName().getChars() );
			gc.drawFilledRectangleRegion( bounds );
		}

		gc.setClip( bounds.x1, bounds.y1, bounds.width, bounds.height );
		{
			Region com_bounds;
			Region clip_region = gc.getClipRegion();
			bool begun_drawing = false;
		
			const Sequence& components = this->getComponents();
			unsigned int max = components.getLength();
			for ( unsigned int i=0; i < max; i++ )
			{
				const Component& com = dynamic_cast<const Component&>( components.elementAt( i ) );
				if ( com.hasChanged() )
				{
					com_bounds = com.getBounds();

					if ( gc.canDraw( com_bounds ) )
					{
						begun_drawing = true;
#ifdef DEBUG_OPENOCL_UI_CONTAINER_DRAW
						fprintf( stderr, "Container::draw: calling draw on %s\n", com.getName().getChars() );
#endif
						gc.setClip( com_bounds.x1, com_bounds.y1, com_bounds.width, com_bounds.height );
						//fprintf( stderr, "%s.Container::draw: (3b)\n", this->getName().getChars() );
						draw_again |= com.drawComponent( gc );
						gc.popClip();
#ifdef DEBUG_OPENOCL_UI_CONTAINER_DRAW
						fprintf( stderr, "Container::draw: finished draw on %s\n", com.getName().getChars() );
#endif
					} else {
						if ( begun_drawing )
						{
							break;
						}
					}
#ifdef DEBUG_OPENOCL_UI_CONTAINER_DRAW_2
					else
					{
						fprintf( stderr, "Container::draw: com is out of clip %s\n", com.getName().getChars() );
					}
#endif
				} else {
#ifdef DEBUG_OPENOCL_UI_CONTAINER_DRAW_2
					fprintf( stderr, "Container::draw: skipping %s (not changed)\n", com.getName().getChars() );
#endif
				}
			}
		}
		gc.popClip();
	}
	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}

/*
unsigned int
Container::getPreferredWidth( const GraphicsContext& gc ) const
{
	Dimensions dim = this->layoutManager->preferredLayoutDimensions( gc, *this, 0xFFFF, 0xFFFF );
	return dim.width;
}

unsigned int
Container::getPreferredHeight( const GraphicsContext& gc ) const
{
	Dimensions dim = this->layoutManager->preferredLayoutDimensions( gc, *this, 0xFFFF, 0xFFFF );
	return dim.height;
}
*/

Dimensions
Container::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	int inset = this->getContentInset();
	int total_padding = inset * 2;

	unsigned int adjusted_width  = width  - total_padding;
	unsigned int adjusted_height = height - total_padding;	

	if ( adjusted_width > width ) adjusted_width = 0;
	if ( adjusted_height > height ) adjusted_height = 0;
	
	Dimensions d = this->layoutManager->preferredLayoutDimensions( gc, *this, adjusted_width, adjusted_height );
	d.width  += total_padding;
	d.height += total_padding;
	
	return d;
}


//------------------------------------------------------------------------
//	public constant methods
//------------------------------------------------------------------------

Region
Container::getLayoutRegion() const
{
	int padding = this->getPadding();

	Region layout_region = this->getBounds().shrunkBy( padding );
	return layout_region;
}

const Sequence&
Container::getComponents() const
{
	return *this->components;
}

const String&
Container::findValue( const String& componentName ) const
throw (NoSuchElementException*)
{
	const String* str = null;

	Iterator* it = this->components->iterator();

	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		if ( 0 == com.getName().compareTo( componentName ) )
		{
			const Textual* textual = dynamic_cast<const Textual*>( &com );
			if ( textual )
			{
				str = &textual->getData();
				break;
			}
			break;
		} else {
			try
			{
				const Container* container = dynamic_cast<const Container*>( &com );
				if ( container )
				{
					str = &container->findValue( componentName );
					break;
				}
				const Control* control = dynamic_cast<const Control*>( &com );
				if ( control )
				{
					str = &control->findValue( componentName );
					break;
				}
			} catch ( NoSuchElementException* ex ) {
				delete ex;
			}
		}
	}
	delete it;

	if ( null == str )
	{
		throw new NoSuchElementException( componentName );
	}
	
	return *str;
}

bool
Container::isRedrawAll() const
{
	return this->redrawAll;
}

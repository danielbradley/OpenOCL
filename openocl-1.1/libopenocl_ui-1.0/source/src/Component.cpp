/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Component.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/UIException.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/RuntimeException.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/RefreshEventListener.h>
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
using namespace openocl::ui;
using namespace openocl::util;

unsigned int Component::eventsFired = 0;
unsigned int Component::eventsForwarded = 0;


Component::Component()
{
	this->name = new String();
	this->listeners = new Sequence();

	this->boundsChanged = true;
	this->containingPanel = null;
	this->eventMask = 0xFFFFFFFF;
	this->hints = LayoutManager::TOP;
	this->changed  = true;
	this->focus    = false;
	this->editMode = false;

	this->edge = 5;
	this->margin = 0;
	this->border = 0;
	this->padding = 5;
	this->lineHeight = 1;
}

Component::Component( const String& name )
{
	this->name = new String( name );
	this->listeners = new Sequence();

	this->boundsChanged = true;
	this->containingPanel = null;
	this->eventMask = 0xFFFFFFFF;
	this->hints = LayoutManager::TOP;
	this->changed  = true;
	this->focus    = false;
	this->editMode = false;

	this->lastPreferredDimensions.width  = 0xFFFFF;
	this->lastPreferredDimensions.height = 0xFFFFF;

	this->edge = 5;
	this->margin = 0;
	this->border = 0;
	this->padding = 5;
	this->lineHeight = 1;
}

Component::~Component()
{
	delete this->listeners;
	delete this->name;
}

//------------------------------------------------------------------------
//	public virtual methods (EventHandler)
//------------------------------------------------------------------------

void
Component::addEventListener( EventListener& anEventListener )
{
	Object* obj = dynamic_cast<Object*>( &anEventListener );
	if ( null != obj ) {
		this->listeners->add( *obj );
	} else {
		throw new UIException( "Null event listener added to component" );
	}
}

void
Component::fireEvent( const Event& anEvent )
{
	Component::eventsFired++;
	bool refresh_event = false;

	if ( Event::ACTION_EVENT == anEvent.getType() )
	{
		if ( dynamic_cast<const ActionEvent&>( anEvent ).getCommand().startsWith( "REFRESH." ) )
		{
			//fprintf( stderr, "Component::fireEvent: REFRESH event\n" );
			refresh_event = true;
		}
	}

	Iterator* it = this->listeners->iterator();
	while ( it->hasNext() )
	{
		Object& obj = const_cast<Object&>( it->next() );
		EventListener& el = dynamic_cast<EventListener&>( obj );
		//fprintf( stderr, "Component::fireEvent(): delivering event\n" );
		//if ( refresh_event )
		//{
		//	fprintf( stderr, "Component::fireEvent: >>> " ); Runtime::printMemorySummary( stderr );
		//}
			el.deliverEvent( anEvent );
		//if ( refresh_event )
		//{
		//	fprintf( stderr, "Component::fireEvent: <<< " ); Runtime::printMemorySummary( stderr );
		//}
		//fprintf( stderr, "Component::fireEvent(): delivered event\n" );
	}
	delete it;
}


void
Component::removeEventListener( EventListener& anEventListener )
{
	unsigned int max = this->listeners->getLength();
	Object& object = dynamic_cast<Object&>( anEventListener );
	Object* obj = &object;
	void* el = (void*) obj;

	for ( unsigned int i=0; i < max; i++ )
	{
		void* obj = (void*) &this->listeners->elementAt( i );
		if ( obj == el )
		{
			this->listeners->remove( i );
			
			//	We recursively call just in case there
			//	are two or more references in this
			//	EventHandler
			this->removeEventListener( anEventListener );
			break;
		}
	}
}

//------------------------------------------------------------------------
//	public virtual methods
//------------------------------------------------------------------------

void
Component::setBounds( int x, int y, unsigned int width, unsigned int height )
{
#ifdef DEBUG_OPENOCL_UI_COMPONENT
	int x2 = x + width - 1;
	int y2 = y + height - 1;
	fprintf( stderr, "%s.Component::setBounds: %i --> %i : %i ^ %i - ( %u, %u )\n", this->getName().getChars(),
			x, x2, y, y2, width, height );
#endif

	if ( (this->bounds.x != x)        ||
	     (this->bounds.y != y )        ||
	     (this->bounds.width != width) ||
	     (this->bounds.height != height) )
	{
		//fprintf( stderr, "Component::setBounds: bounds changed\n" );
		this->bounds.x = x;
		this->bounds.y = y;
		this->bounds.width = width;
		this->bounds.height = height;
		this->setBoundsChanged( true );
		this->setChanged( true );
	}
}

void
Component::setEventMask( flags anEventMask )
{
	this->eventMask = anEventMask;
}

void
Component::setContainingPanel( Panel& aPanel )
{
	if ( null != &aPanel )
	{
		this->containingPanel = &aPanel;
	} else {
		fprintf( stderr, "Component::setContainingPanel: Attempting to set to null\n" );
		abort();
	}
}

void
Component::resetContainingPanel()
{
	this->containingPanel = null;
}

void
Component::doLayout( const GraphicsContext& gc )
{
	Region bounds = this->getBounds();
	Region box;
	unsigned int e = this->edge;

	int cx = bounds.x1 + bounds.width/2;
	int cy = bounds.y1 + bounds.height/2;
	
	Dimensions d = this->lastPreferredDimensions;

	unsigned int total_edge = e * 2;
	unsigned int w = d.width  - total_edge;
	unsigned int h = d.height - total_edge;
	
	switch ( this->hints & LayoutManager::ALIGN )
	{
	case LayoutManager::LEFT:
		box.x1 = bounds.x1 + e;
		box.width = w;
		box.x2 = box.x1 + box.width - 1;
		break;
	case LayoutManager::RIGHT:
		box.x2 = bounds.x2 - e;
		box.width = w;
		box.x1 = box.x2 - box.width + 1;
		break;
	case LayoutManager::CENTERED:
		box.width = w;
		box.x1 = cx - w/2;
		box.x2 = box.x1 + box.width - 1;
		break;
	default:
		box.x1 = bounds.x1 + e;
		box.x2 = bounds.x2 - e;
		box.width = box.x2 - box.x1 + 1;
	}

	switch ( this->hints & LayoutManager::VALIGN )
	{
	case LayoutManager::TOP:
		box.y1 = bounds.y1 + e;
		box.height = h;
		box.y2 = box.y1 + box.height - 1;
		break;
	case LayoutManager::BOTTOM:
		box.y2 = bounds.y2 - e;
		box.height = h;
		box.y1 = box.y2 - box.height + 1;
		break;
	case LayoutManager::CENTERED:
		box.height = h;
		box.y1 = cy - h/2;
		box.y2 = box.y1 + box.height - 1;
		break;
	default:
		box.y1 = bounds.y1 + e;
		box.y2 = bounds.y2 - e;
		box.height = box.y2 - box.y1 + 1;
	}	
	this->edgeRegion = box;
}

void
Component::forwardEvent( const Event& anEvent )
{
	//fprintf( stderr, "%s.Component::forwardEvent()\n", this->getName().getChars() );

	flags type = anEvent.getType();

	//if ( 0 != (type & this->eventMask) )
	//{
		Component::eventsForwarded++;

		bool changed = false;
		
		bool change_event  = (Event::CHANGE_EVENT  == (Event::CHANGE_EVENT & type));
		bool focus_event   = (Event::FOCUS_EVENT   == (Event::FOCUS_EVENT & type));
		bool mouse_event   = (Event::MOUSE_EVENT   == (Event::MOUSE_EVENT & type));
		bool keydown_event = (Event::KEYDOWN_EVENT == (Event::KEYDOWN_EVENT & type));

		if ( change_event )
		{
			this->setChanged( true );
		}
		else if ( focus_event )
		{
			const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
			switch ( fe.getFocusEventType() )
			{
			case FocusEvent::CIRCULATE_FORWARD:
			case FocusEvent::CIRCULATE_BACKWARD:
			case FocusEvent::TAKE_FOCUS:
				this->focus = true;
				break;
			case FocusEvent::LOSE_FOCUS:
				//fprintf( stderr, "Component::forwardEvent: lost focus\n" );
				this->focus    = false;
				this->editMode = false;
			}
			changed = true;
			
			this->processEvent( fe );
		}
		else if ( keydown_event )
		{
			if ( this->hasFocus() )
			{
				const KeyEvent& ke = dynamic_cast<const KeyEvent&>( anEvent );
				mask key = ke.getKey();

				if ( this->editMode )
				{
					if ( KeyEvent::ESC == key )
					{
						this->editMode = false;
						changed = true;
					}
					this->processEvent( anEvent );
				}
				else if ( (KeyEvent::PRINTABLE_MASK & key) && ('\t' == key) )
				{
					if (
							(KeyEvent::SHIFT_LEFT  == (KeyEvent::SHIFT_LEFT & ke.getModifiers()))
						||	(KeyEvent::SHIFT_RIGHT == (KeyEvent::SHIFT_RIGHT & ke.getModifiers()))
						||	(InputEvent::SHIFT     == (InputEvent::SHIFT & ke.getModifiers()))
					)
					{
						FocusEvent focus_event( *this, 0, FocusEvent::CIRCULATE_BACKWARD );
						this->fireEvent( focus_event );
						//fprintf( stderr, "Component::forwardEvent: TAB (SHIFT)\n" );
					}
					else
					{
						FocusEvent focus_event( *this, 0, FocusEvent::CIRCULATE_FORWARD );
						this->fireEvent( focus_event );
						//fprintf( stderr, "Component::forwardEvent: TAB\n" );
					}
					changed = true;
				}
				else
				{
					this->processEvent( anEvent );
				}
			}
		}
		else if ( mouse_event )
		{
			//	If a mouse event occurred outside of this componet, make this
			//	component lose focus.

			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			bool contained = this->contains( me.getX(), me.getY() );
			if ( !contained && this->focus )
			{
				this->focus = false;
				changed = true;
			}
			else if ( contained && !this->focus )
			{
				//  XXX Should components always gain focus when tabbed to, or should
				//	this be determined by the component (eg. CheckBox), at the moment
				//	I will leave this commented out so that the component can decide.
				//
				this->focus = true;
				changed = true;
			}
			this->processEvent( anEvent );
		}
		else
		{
			this->processEvent( anEvent );
		}
	//}

	//
	//	I think that the decision to redraw should be left up to the individual component.
	//	Often a change of focus will not cause a component to change visually, so therefore
	//	it can just provide a very unnecessary redraw of things.
	//
	//if ( changed )
	//{
	//	this->fireChangeEvent();
	//	this->getContainingPanel().redraw();
	//}
}

void
Component::processEvent( const Event& anEvent )
{
	//	This method is overriden by subclasses
	//	that want to perform actions based upon
	//	received events. For example a button
	//	that handles MouseEvents.
}

//------------------------------------------------------------------------
//	public methods
//------------------------------------------------------------------------

void
Component::fireRefreshEvent( const Event& anEvent )
{
	Iterator* it = this->listeners->iterator();
	while ( it->hasNext() )
	{
		Object& obj = const_cast<Object&>( it->next() );
		RefreshEventListener* rel = dynamic_cast<RefreshEventListener*>( &obj );
		if ( rel )
		{
			rel->deliverRefreshEvent( anEvent );
			break;
		}
	}
	delete it;
}

void
Component::fireChangeEvent( unsigned int value )
{
	//	A ChangeEvent is passed back up the container/component hierarchy.
	//	It indicates to each container that a component has changed and therefore
	//	that it needs to be redrawn.

	this->setChanged( true );
	ChangeEvent ce( *this, 0, value );
	//ChangeEvent ce( *this );
	this->fireEvent( ce );
}

void
Component::forwardChangeEvent()
{
	this->setChanged( true );
	ChangeEvent ce( *this );
	this->forwardEvent( ce );
}

void
Component::forwardRedrawAllChangeEvent()
{
	ChangeEvent ce( *this, 0, ChangeEvent::REDRAW_ALL );
	this->setChanged( true );
	this->forwardEvent( ce );
}

void
Component::setBoundsChanged( bool haveChanged )
{
	this->boundsChanged = haveChanged;

	//
	//	Is this necessary, this causes an awful lot of events
	//	when a RELAYOUT event is delivered upwards.
	//if( haveChanged )
	//{
	//	this->fireChangeEvent();
	//}
}

void
Component::setChanged( bool changed ) const
{
	const_cast<Component*>( this )->changed = changed;

	#ifdef DEBUG_OPENOCL_UI_COMPONENT_SETCHANGED
	if ( changed )
	{
		fprintf( stderr, "Component::setChanged( true ): %s\n", this->getName().getChars() );
	}
	#endif
}

void
Component::setEditMode( bool editMode )
{
	this->editMode = editMode;
}

void
Component::setComponentLayoutHints( flags layoutHints )
{
	this->hints = layoutHints;
}

void
Component::setName( const String& name )
{
	delete this->name;
	this->name = new String( name );
}

void
Component::setBoundsX( int x )
{
	this->bounds.x = x;
}

void
Component::setBoundsY( int y )
{
	this->bounds.y = y;
}

void
Component::setEdge( unsigned int size )
{
	this->edge = size;
}

void
Component::setMargin( unsigned int size )
{
	this->margin = size;
}

void
Component::setBorder( unsigned int size )
{
	this->border = size;
}

void
Component::setPadding( unsigned int size )
{
	this->padding = size;
}

void
Component::setLineHeight( unsigned int size )
{
	this->lineHeight = size;
}

//------------------------------------------------------------------------
//	public virtual const methods
//------------------------------------------------------------------------

bool
Component::drawComponent( GraphicsContext& gc ) const
{
	bool redraw = false;
	if ( this->hasChanged() )
	{
		#ifdef DEBUG_OPENOCL_UI_COMPONENT_DRAWCOMPONENT
		fprintf( stderr, "Component::drawComponent: %s\n", this->getName().getChars() );
		#endif
	
		if ( false ) // Add check if draw is a plugin
		{
		
		} else {
			redraw = this->draw( gc );
		}
	}
	return redraw;
}

bool
Component::draw( GraphicsContext& gc ) const
{
	//	This method just draws a test box and grid and text.
	//	A subclass can thus call this method inorder to test their position
	//	on the screen. It is also useful for testing UI providers.

	if ( 1 )
	{
		int x  = this->getX();
		int y  = this->getY();
		unsigned int w = this->getWidth();
		unsigned int h = this->getHeight();

		int fx = x + (w - 1);
		int fy = y + (h - 1);

		gc.setForeground( Color::BLUE );
		gc.setBackground( Color::BLUE );
		gc.drawFilledRectangle( x, y, w, h );
		
		gc.setForeground( Color::WHITE );

		gc.drawLine( x, y, x, fy );
		gc.drawLine( x, y, fx, y );
		gc.drawLine( x, fy, fx, fy );
		gc.drawLine( fx, y, fx, fy );

		gc.drawString( this->name->getChars(), x + 11, y + 20 );

	}
	return false;
}

Coordinates
Component::getAbsoluteCoordinates() const
{
	Coordinates outer  = this->containingPanel->getOuterCoordinates();
	Coordinates offset = this->containingPanel->getOffsetCoordinates();
	Coordinates absolute;
	absolute.x = outer.x + offset.x + this->getX();
	absolute.y = outer.y + offset.y + this->getY();
	return absolute;
}

int
Component::getAbsoluteX() const
{
	unsigned int absolute_x = 0;
	if ( this->containingPanel )
	{
		absolute_x += this->containingPanel->getAbsoluteX();
	}
	absolute_x += this->getX();

	return absolute_x;
}

int
Component::getAbsoluteY() const
{
	unsigned int absolute_y = 0;
	if ( this->containingPanel )
	{
		absolute_y += this->containingPanel->getAbsoluteY();
	}
	absolute_y += this->getY();

	return absolute_y;
}

Region
Component::getBounds() const
{
	Region reg;
	reg.x1 = bounds.x;
	reg.width = bounds.width;
	reg.x2 = reg.x1 + reg.width - 1;
	reg.y1 = bounds.y;
	reg.height = bounds.height;
	reg.y2 = reg.y1 + reg.height - 1;
	
	return reg;
}

Region
Component::getEdgeRegion() const
{
	return this->getBounds().shrunkBy( edge ); 
}

Region
Component::getMarginRegion() const
{
	return this->getBounds().shrunkBy( edge + margin );
}

Region
Component::getBorderRegion() const
{
	return this->getBounds().shrunkBy( this->edge + this->margin + this->border );
}

Region
Component::getContentRegion() const
{
	return this->getBounds().shrunkBy( this->edge + this->margin + this->border + this->padding );
}

Region
Component::getBox() const
{
	return this->getEdgeRegion();
}

unsigned int
Component::getDrawableWidth() const
{
	return this->bounds.width;
}

unsigned int
Component::getDrawableHeight() const
{
	return this->bounds.height;
}

/*
Dimensions
Component::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d;
	d.width = this->getPreferredWidth( gc );
	d.height = this->getPreferredHeight( gc );
	return d;
}
*/

//------------------------------------------------------------------------
//	public const methods
//------------------------------------------------------------------------

const Panel&
Component::getContainingPanel() const
throw (NoSuchElementException*)
{
	if ( !this->containingPanel )
	{
		throw new NoSuchElementException();
	}
	return *this->containingPanel;
}

Panel&
Component::getContainingPanel()
throw (NoSuchElementException*)
{
	if ( !this->containingPanel )
	{
		throw new NoSuchElementException();
	}
	return *this->containingPanel;
}

const String&
Component::getName() const
{
	return *this->name;
}

unsigned int
Component::getEdge() const
{
	return this->edge;
}

unsigned int
Component::getMargin() const
{
	return this->margin;
}

unsigned int
Component::getBorder() const
{
	return this->border;
}

unsigned int
Component::getPadding() const
{
	return this->padding;
}

unsigned int
Component::getContentInset() const
{
	return (this->edge + this->margin + this->border + this->padding);
}

unsigned int
Component::getLineHeight() const
{
	return this->lineHeight;
}

int
Component::getX() const
{
	return this->bounds.x;
}

int
Component::getY() const
{
	return this->bounds.y;
}

unsigned int
Component::getWidth() const
{
	return this->bounds.width;
}

unsigned int
Component::getHeight() const
{
	return this->bounds.height;
}

bool
Component::contains( int anX, int aY ) const
{
	bool contained = false;

	int maxX = this->bounds.x + this->bounds.width;
	int maxY = this->bounds.y + this->bounds.height;

	if ( (anX >= this->bounds.x) &&
	     (anX < maxX)     &&
	     (aY >= this->bounds.y)  &&
	     (aY < maxY ) )
	{
		contained = true;
	}
	return contained;
}

bool
Component::haveBoundsChanged() const
{
	return this->boundsChanged;
}

bool
Component::hasChanged() const
{
	return this->changed;
}

bool
Component::hasContainingPanel() const
{
	return this->containingPanel;
}

bool
Component::isEditMode() const
{
	return this->editMode;
}

//------------------------------------------------------------------------
//	protected constant methods
//------------------------------------------------------------------------

void
Component::drawInward3DBox( GraphicsContext& gc, const Region& aRegion, const Color& light, const Color& shade ) const
{
	gc.setForeground( shade );
	gc.drawRectangleRegion( aRegion );
	gc.setForeground( light );
	gc.drawLine( aRegion.x2, aRegion.y2, aRegion.x1, aRegion.y2 );
	gc.drawLine( aRegion.x2, aRegion.y2, aRegion.x2, aRegion.y1 );
}


void
Component::drawOutward3DBox( GraphicsContext& gc, const Region& aRegion, const Color& light, const Color& shade ) const
{
	gc.setForeground( shade );
	gc.drawRectangleRegion( aRegion );
	gc.setForeground( light );
	gc.drawLine( aRegion.x1, aRegion.y1, aRegion.x1, aRegion.y2 - 1 );
	gc.drawLine( aRegion.x1, aRegion.y1, aRegion.x2 - 1, aRegion.y1 );
}

bool
Component::hasFocus() const
{
	return this->focus;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/TabbedView.h"

#include "openocl/ui/ColumnLayout.h"
#include "openocl/ui/Cursor.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/Number.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/formats/XPixmap.h>
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

unsigned int TabbedView::tabWidth = 150;
unsigned int TabbedView::tabHeight = 30;

TabbedView::TabbedView()
{
	this->setPadding( 0 );
	this->nrOfComponents = 0;
	this->setName( "TabbedView" );

	this->bounds   = new Region();
	this->outline  = new Region();
	this->closeTab = new Region();
	this->tabarea  = new Region();
	this->active   = new Region();

	this->activeId = 0;
}

TabbedView::~TabbedView()
{
	delete this->bounds;
	delete this->outline;
	delete this->closeTab;
	delete this->tabarea;
	delete this->active;
}

void
TabbedView::doLayout( const GraphicsContext& gc )
{
	unsigned int padding = this->getPadding();
	
	Region bounds;
	Region outline;
	Region tabclose;
	Region tabarea;
	Region active;

	bounds.x1     = this->getX();
	bounds.y1     = this->getY();
	bounds.width  = this->getWidth();
	bounds.height = this->getHeight();
	bounds.x2 = bounds.x1 + bounds.width - 1;
	bounds.y2 = bounds.y1 + bounds.height - 1;
	
	tabclose.x1 = bounds.x1;
	tabclose.width = tabHeight;
	tabclose.x2 = tabclose.x1 + tabclose.width - 1;
	tabclose.y1 = bounds.y1;
	tabclose.height = tabHeight;
	tabclose.y2 = tabclose.y1 + tabclose.height - 1;

	tabarea.x1 = tabclose.x1;// tabclose.x2 + 1;
	tabarea.x2 = bounds.x2;
	tabarea.width = tabarea.x2 - tabarea.x1 + 1;
	tabarea.y1 = tabclose.y1;
	tabarea.height = tabclose.height + 1;
	tabarea.y2 = tabarea.y1 + tabarea.height - 1;

	outline = bounds;
	outline.y1 = tabarea.y2 + 1;
	outline.height = outline.y2 - outline.y1 + 1;
	
	active.x1 = outline.x1 + padding;
	active.x2 = outline.x2 - padding;
	active.width = active.x2 - active.x1 + 1;
	active.y1 = tabarea.y2 + padding;
	active.y2 = outline.y2 - padding;
	active.height = active.y2 - active.y1 + 1;

	if ( 0 < this->nrOfComponents )
	{
		Component& com = dynamic_cast<Component&>( this->getComponents().elementAt( this->activeId ) );
		com.setBounds( active.x1, active.y1, active.width, active.height );
		com.doLayout( gc );
	}

	*this->bounds   = bounds;
	*this->outline  = outline;
	*this->closeTab = tabclose;
	*this->tabarea  = tabarea;
	*this->active   = active;
	
	this->setBoundsChanged( false );
}

//	Override forwardEvent in Container so that only
//	the active tab receives events.

void
TabbedView::forwardEvent( const Event& anEvent )
{
	bool action  = (Event::ACTION_EVENT == (Event::ACTION_EVENT & anEvent.getType()) );
	bool changed = (Event::CHANGE_EVENT == (Event::CHANGE_EVENT & anEvent.getType()) );
	if ( changed )
	{
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		if ( ChangeEvent::REDRAW_ALL == ce.getValue() )
		{
			this->setRedrawAll( true );
		}
		this->setChanged( true );
	}
	else if ( action )
	{
		//	This allows REFRESH initiatied actions events to be propagated to all Documents
		//	not just active ones.
		this->Container::forwardEvent( anEvent );
	}
	this->processEvent( anEvent );
}

void
TabbedView::processEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	
	Region closetab = *this->closeTab;
	Region tabarea  = *this->tabarea;
	Region active   = *this->active;
	
	if ( 0 < this->nrOfComponents )
	{
		Object& obj = this->getComponents().elementAt( this->activeId );
		Component& com = dynamic_cast<Component&>( obj );
		
		if ( Event::POINTER_EVENT == (Event::POINTER_EVENT & type) )
		{
			const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
			int pe_x = pe.getX();
			int pe_y = pe.getY();
			
			if ( active.contains( pe_x, pe_y ) )
			{
				com.forwardEvent( anEvent );
			}
			else if ( Event::MOUSE_EVENT == (Event::MOUSE_EVENT & type) )
			{
				const MouseEvent& me = dynamic_cast<const MouseEvent&>( pe );
				flags button = me.getButton();
				if ( button == (MouseEvent::BUTTON1_RELEASE & button) )
				{
					//if ( closetab.contains( pe_x, pe_y ) )
					//{
					//	Component& com = dynamic_cast<Component&>( this->getComponents().elementAt( this->activeId ) );
					//	ActionEvent ae( *this, 0, "CLOSE.openocl::ui::TabbedView", com.getName().getChars() );
					//	this->fireEvent( ae );
					//	this->getContainingPanel().relayout();
					//}
					//else
					if ( tabarea.contains( pe_x, pe_y ) )
					{
						unsigned int tab_width = this->tabWidth;
						if ( 0 < this->nrOfComponents )
						{
							unsigned int width = tabarea.width / this->nrOfComponents;
							tab_width = Math::min( width, tab_width );
							unsigned int provisional = pe_x / tab_width;
							if ( provisional < this->nrOfComponents )
							{
								this->activeId = provisional;
								{
									Component& com = dynamic_cast<Component&>( this->getComponents().elementAt( this->activeId ) );
									ActionEvent ae( *this, 0, "openocl::ui::TabbedView", com.getName().getChars() );
									this->fireEvent( ae );
									FocusEvent lose_focus( *this, 0, FocusEvent::LOSE_FOCUS );
									this->forwardEvent( lose_focus );
									
									//com.forwardRedrawAllChangeEvent(); ///
									this->forwardRedrawAllChangeEvent();
								}
								this->forwardChangeEvent();
								this->setChanged( true );
								this->fireEvent( ChangeEvent( *this ) );
								this->getContainingPanel().relayout();
							}
						}
					}
				}
			}
			
			//	XXX Debug (should be unnecessary)
			//this->getContainingPanel().relayout();
		} else {
			bool changed = (Event::CHANGE_EVENT == (Event::CHANGE_EVENT & anEvent.getType()) );
			if ( changed )
			{
				//fprintf( stderr, "TabbedView::processEvent: forwarding change event to active tab\n" );
			}
			com.forwardEvent( anEvent );
		}
	}
	
}

void
TabbedView::add( Component* component )
{
	this->Container::add( component );
	this->activeId = this->nrOfComponents;
	this->nrOfComponents++;
	try
	{
		this->fireChangeEvent();
		this->forwardRedrawAllChangeEvent(); //this->setRedrawAll( true );
		this->getContainingPanel().relayout();
	} catch ( NoSuchElementException* ex ) {
		abort();
		delete ex;
	}
}

void
TabbedView::add( Component& component )
{
	this->Container::add( component );
	this->activeId = this->nrOfComponents;
	this->nrOfComponents++;
	try
	{
		this->fireChangeEvent();
		this->forwardRedrawAllChangeEvent(); //this->setRedrawAll( true );
		this->getContainingPanel().relayout();
	} catch ( NoSuchElementException* ex ) {
		abort();
		delete ex;
	}
}

void
TabbedView::remove( Component& component )
{
	this->Container::remove( component );
	this->activeId = 0;
	this->nrOfComponents--;

	try
	{
		this->fireChangeEvent();
		this->forwardRedrawAllChangeEvent(); //this->setRedrawAll( true );
		this->getContainingPanel().relayout();
	} catch ( NoSuchElementException* ex ) {
		abort();
		delete ex;
	}
}

Dimensions
TabbedView::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d( width, height );
	return d;
}

bool
TabbedView::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();

	Color original_background( gc.getBackground() );

	Region bounds   = *this->bounds;
	Region outline  = *this->outline;
	Region closetab = *this->closeTab;
	Region tabarea  = *this->tabarea;
	Region active   = *this->active;

	//	Export tab_width and components into namespace
	//
	unsigned int tab_width = 0;
	const Sequence& components = this->getComponents();
	if ( changed || redraw_all )
	{
		tab_width = this->tabWidth;
		if ( 0 < this->nrOfComponents )
		{
			unsigned int width = tabarea.width / this->nrOfComponents;
			tab_width = Math::min( width, tab_width );
		}
	}

	Region tab_tray = tabarea.shrunkBy( 4 );
	tab_tray.width = tab_width * this->nrOfComponents + 2;
	tab_tray.x2 = tab_tray.x1 + tab_tray.width - 1;

	//	If needed repaint the dark grey background (beneath the active tab)
	//
	//	Uses: bounds
	//
	if ( redraw_all )
	{
		//	Paint background
		gc.setBackground( Color::DIALOG );
		gc.setFill( Color::DIALOG );
		gc.drawFilledRectangleRegion( bounds );
	}

	//	Draw close cross beside tabs
	//
	//	Uses: closetab
	//
	if ( false )// changed && redraw_all )
	{
		//	Draw tabs
		if ( 0 < this->nrOfComponents )
		{
				//gc.setForeground( Color::TOOLBAR );
				//gc.setFill( Color::TOOLBAR );
				//gc.drawFilledRectangle( closetab.x1, closetab.y1, closetab.width, closetab.height );
			
				//gc.setForeground( Color::SHADOW );
				//gc.drawRectangle( closetab.x1, closetab.y1, closetab.width, closetab.height );


				//	Draw white cross in close tab area.

				gc.setForeground( Color::WHITE );
				gc.drawLine( closetab.x1 + 10, closetab.y1 + 10, closetab.x2 - 10, closetab.y2 - 10 );
				gc.drawLine( closetab.x1 + 9, closetab.y1 + 10, closetab.x2 - 11, closetab.y2 - 10 );
				gc.drawLine( closetab.x2 - 10, closetab.y1 + 10, closetab.x1 + 10, closetab.y2 - 10 );
		}
	}
	
	//	Draw tab of each component.
	//
	//	Uses: tabarea, components, 
	if ( redraw_all )
	{
		//gc.setFill( Color::SCROLLBAR );
		//gc.drawFilledRectangleRegion( tab_tray );
		//this->drawInward3DBox( gc, tab_tray, Color::SHINE2, Color::SHADE1 );

		int x = tab_tray.x1 + 1;
		for ( unsigned int i=0; i < this->nrOfComponents; i++ )
		{
			const Component& com = dynamic_cast<const Component&>( components.elementAt( i ) );
		
			//	Draw tab
			{
				Region tab;
				tab.x1 = x;
				tab.width = tab_width + 1;
				tab.x2 = tab.x1 + tab.width - 1;
				tab.y1 = tab_tray.y1 + 2;
				tab.y2 = tabarea.y2;
				tab.height = tab.y2 - tab.y1 + 1;

				gc.setFill( Color::TOOLBAR );
				gc.drawFilledRectangleRegion( tab );
				gc.setForeground( Color::SHADE2 );
				gc.drawRectangleRegion( tab );
				
				//this->drawOutward3DBox( gc, tab, Color::SHINE2, Color::SHADE1 );

		
				//gc.setForeground( Color::SHADOW );
				//gc.drawLine( tab.x2, tab.y2, tab.x1, tab.y2 ); 
				//gc.drawLine( tab.x2, tab.y2, tab.x2, tab.y1 ); 
				//gc.setForeground( Color::SHINY );
				//gc.drawLine( tab.x1, tab.y1, tab.x1, tab.y2 ); 
				//gc.drawLine( tab.x1, tab.y1, tab.x2, tab.y1 ); 

				//gc.drawRectangle( x, tabarea.y1, this->tabWidth, tabarea.height );
				gc.setForeground( Color::INACTIVE_TEXT );
				gc.drawString( com.getName(), tab.x1 + 10, tab.y2 - 7 );
				x += tab_width;
			}
		}
	}
	
	//	Fill background area of content area.
	//
	//	Uses: outline

	if ( redraw_all )
	{
		if ( 0 < this->nrOfComponents )
		{
			//	Draw content area
			gc.setFill( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( outline );
		}
	}

	if ( changed )
	{
		if ( 0 < this->nrOfComponents )
		{
			const Component& com = dynamic_cast<const Component&>( components.elementAt( this->activeId ) );
			if ( com.hasChanged() )
			{
				com.draw( gc );
			}
		}
	}

	if ( changed )
	{
		//this->drawOutward3DBox( gc, bounds, Color::SHINE1, Color::SHADE2 );
		this->drawOutward3DBox( gc, tabarea, Color::SHINE1, Color::SHADE2 );
		this->drawOutward3DBox( gc, outline, Color::SHINE1, Color::SHADE2 );
	}

	//	Redraw active tab in front of other tabs and name in black
	//
	//	Uses: tabarea, componets
	//
	if ( changed )
	{
		if ( 0 < this->nrOfComponents )
		{
			//	Draw active tab
			Region tab;
			tab.x1 = tab_tray.x1 + 1 + (tab_width*this->activeId );
			tab.width = tab_width + 1;
			tab.x2 = tab.x1 + tab.width - 1;
			tab.y1 = tab_tray.y1;
			tab.y2 = tabarea.y2;
			tab.height = tab.y2 - tab.y1 + 1;

			//	Color tab SCROLLBAR.
			gc.setBackground( Color::SCROLLBAR );
			gc.setFill( Color::SCROLLBAR );
			gc.drawFilledRectangleRegion( tab );

			//	Draw dark line around tab
			gc.setForeground( Color::SHADE2 );
			gc.drawRectangleRegion( tab );

			const Component& com = dynamic_cast<const Component&>( components.elementAt( this->activeId ) );
			gc.setForeground( Color::TEXT );
			gc.drawString( com.getName(), tab.x1 + 10, tab.y2 - 9 );
		}
	}

	if ( changed )
	{
		if ( 0 == this->nrOfComponents )
		{
			gc.setFill( Color::SHADOW_GREY );
			gc.drawFilledRectangleRegion( bounds );
		}
	}

	gc.setBackground( original_background );

	this->setChanged( false );
	this->setRedrawAll( false );
	return draw_again;
}

Component&
TabbedView::getActiveComponent()
throw (NoSuchElementException*)
{
	try
	{
		return dynamic_cast<Component&>( this->getComponents().elementAt( this->activeId ) );
	} catch (IndexOutOfBoundsException* ex ) {
		delete ex;
		throw new NoSuchElementException();
	}
}

const Component&
TabbedView::getActiveComponent() const
throw (NoSuchElementException*)
{
	try
	{
		return dynamic_cast<const Component&>( this->getComponents().elementAt( this->activeId ) );
	} catch (IndexOutOfBoundsException* ex ) {
		delete ex;
		throw new NoSuchElementException();
	}
}

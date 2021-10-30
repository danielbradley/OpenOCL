/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Button.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Menu.h"
#include "openocl/ui/MenuGroup.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/PopupMenu.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

Menu::Menu( const String& name, const String& action, const String& label )
: MenuItem( name, action, label )
{
	Color shade( Color::SHADE2 );
	this->menuGroup = new MenuGroup();
	this->menuGroup->setOuterShade( shade );
	this->menuGroup->addEventListener( *this );
	this->popup = null;
	this->selected = false;
	this->removePopup = false;
}

Menu::~Menu()
{
	this->menuGroup->removeEventListener( *this );
	delete this->popup;
	delete this->menuGroup;
}

//-------------------------------------------------------------------
//	public virtual methods (EventListener)
//-------------------------------------------------------------------

void
Menu::deliverEvent( const Event& anEvent )
{
	if ( Event::ACTION_EVENT == anEvent.getType() )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		//const String& command = ae.getCommand();

		if ( this->popup && ae.getCommand().endsWith( "CLICKED" ) )
		{
			this->selected = false;
			this->popup->setVisible( false );
			this->removePopup = true;

			this->fireChangeEvent();
			this->getContainingPanel().relayout();
		}
	}

	this->fireEvent( anEvent );
}

void
Menu::processEvent( const Event& anEvent )
{
	Region bounds = this->getBounds();
	int event_type = anEvent.getType();

	bool fire_change = false;

	switch ( event_type )
	{
	case Event::FOCUS_EVENT:
		{
			const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
			switch ( fe.getFocusEventType() )
			{
			case FocusEvent::LOSE_FOCUS:
				ActionEvent ae( *this, 0, "INACTIVE.openocl::ui::Menu.PRESS" );
				this->fireEvent( ae );
				this->hidePopup();
				fire_change = true;
			}
		}		
		break;
	case Event::MOUSE_EVENT:
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			if ( bounds.contains( me.getX(), me.getY() ) )
			{
				if ( (MouseEvent::BUTTON1_PRESS == me.getButton()) && !this->selected )
				{
					ActionEvent ae( *this, 0, "ACTIVE.openocl::ui::Menu.PRESS" );
					this->fireEvent( ae );
					
					this->showPopup();
					fire_change = true;
				}
				else if ( (MouseEvent::BUTTON1_PRESS == me.getButton()) && this->selected )
				{
					ActionEvent ae( *this, 0, "INACTIVE.openocl::ui::Menu.PRESS" );
					this->fireEvent( ae );
					this->hidePopup();
					fire_change = true;
				}
			}
			else if ( this->popup )
			{
				ActionEvent ae( *this, 0, "INACTIVE.openocl::ui::Menu.PRESS" );
				this->fireEvent( ae );
				this->hidePopup();
				fire_change = true;
			}
		}
		break;
	case Event::CLOSE_EVENT:
		{
			//fprintf( stderr, "Menu::processEvent: CLOSE_EVENT\n" );
			delete this->popup;
			this->popup = null;
		}
	}

	if ( fire_change )
	{
		this->fireChangeEvent();
		//XXX-Debug this->getContainingPanel().relayout();
		this->getContainingPanel().redraw();
	}
}

void
Menu::setContainingPanel( Panel& aPanel )
{
	this->MenuItem::setContainingPanel( aPanel );

	if ( this->popup && !this->selected )
	{
		this->popup->remove( *this->menuGroup );
		delete this->popup;
		this->popup = null;
	}
}	

void
Menu::doLayout( const GraphicsContext& gc )
{
	this->MenuItem::doLayout( gc );

	if ( this->removePopup )
	{
		this->hidePopup();
		this->removePopup = false;
	}
}

//------------------------------------------------------------------------------------
//	public virtual constant methods (Component)
//------------------------------------------------------------------------------------

Dimensions
Menu::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d = this->MenuItem::getPreferredDimensions( gc, width, height );
	//d.width = Math::max( d.width, this->menuGroup->getPreferredDimensions( gc, width, height ).width );
	return d;
}

bool
Menu::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	Region bounds = this->getBounds();

	Color original_background( gc.getBackground() );

	if ( this->selected )
	{

		gc.setFill( Color::SCROLLBAR );
		gc.setBackground( Color::SCROLLBAR );
		gc.drawFilledRectangleRegion( bounds );

		//bounds.y1++;
		//bounds.y2--;
		//bounds.height -= 2;
		
		//gc.setForeground( Color::SHADOW_GREY );
		//gc.drawLine( bounds.x1 + 1, bounds.y1, bounds.x2, bounds.y1 );
		//gc.drawLine( bounds.x1 + 1, bounds.y1, bounds.x1 + 1, bounds.y2 );
		//gc.drawLine( bounds.x2, bounds.y1, bounds.x2, bounds.y2 );


//		gc.setForeground( Color::SHADOW_GREY );
//		gc.drawRectangleRegion( bounds );
//		this->drawOutward3DBox( gc, bounds, Color::SHADE2, Color::SHINE1 );

		this->drawInward3DBox( gc, bounds, Color::SHINE2, Color::SHADE2 );
		
		//gc.setForeground( Color::SHADOW );
		//gc.drawRectangleRegion( bounds );
		//gc.drawLine( bounds.x2, bounds.y1, bounds.x2, bounds.y2 );
		//gc.setForeground( Color::SHINY );
		//gc.drawLine( bounds.x1, bounds.y1, bounds.x1, bounds.y2 );
		//gc.drawLine( bounds.x1, bounds.y1, bounds.x2, bounds.y1 );
	}
	//else {
	//	gc.setForeground( Color::TOOLBAR );
	//	gc.drawRectangleRegion( bounds );
	//}	

	this->MenuItem::draw( gc );
	this->setChanged( false );
	gc.setBackground( original_background );

	return draw_again;
}

//------------------------------------------------------------------------------------
//	public methods (Menu)
//------------------------------------------------------------------------------------

bool
Menu::activateIfContains( int x, int y )
{
	Region bounds = this->getBounds();
	bool contains = bounds.contains( x, y );
	if ( contains )
	{
		this->showPopup();
	} else {
		this->hidePopup();
	}
	return contains;
}

void
Menu::deactivate()
{
	this->hidePopup();
}

void
Menu::addMenuItem( MenuItem* aMenuItem )
{
	this->menuGroup->add( aMenuItem );
}

void
Menu::addMenuItem( MenuItem& aMenuItem )
{
	this->menuGroup->add( aMenuItem );
}

void
Menu::remove( MenuItem& aMenuItem )
throw (NoSuchElementException*)
{
	this->menuGroup->remove( aMenuItem );
}

void
Menu::showPopup()
{
	//unsigned int padding = Textual::padding;
	
	Region box = this->getBounds();

	if ( this->removePopup && this->popup )
	{
		fprintf( stderr, "Menu::showPopup: Preexisting popup encountered!\n" );
		this->hidePopup();
		this->removePopup = false;
	}

	if ( !this->popup )
	{
		this->selected = true;
		if ( this->hasContainingPanel() )
		{
			this->popup = new PopupMenu( this->getContainingPanel() );
			this->popup->setName( "popup" );
			this->popup->add( *this->menuGroup );
		}

		if ( this->popup )
		{
			Coordinates c = this->getAbsoluteCoordinates();
		
			int x = c.x;
			int y = c.y + box.height - 1;
			unsigned int width  = Math::max( box.width, this->popup->getPreferredWidth() );
			unsigned int height = this->popup->getPreferredHeight();

			this->popup->requestGeometry( x, y, width, height, 0 );

			//this->popup->toFront();
			//this->popup->show( Panel::HIDDEN );
			this->popup->show( Panel::SHOW );
			this->popup->getTopPanel().relayout();
		}

		this->fireChangeEvent();
		this->getContainingPanel().redraw();
	}
}

void
Menu::hidePopup()
{
	//fprintf( stderr, "Menu::hidePopup()\n" );
	if ( this->popup )
	{
		this->popup->setVisible( false );
		this->popup->remove( *this->menuGroup );
		delete this->popup;
		this->popup = null;
	}
	this->selected = false;
	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}


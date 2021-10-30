/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/MenuGroup.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SidewaysLayout.h"
#include "openocl/ui/StackedLayout.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/ui/Menu.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;

MenuGroup::MenuGroup( flags layout ) : OpaqueContainer( Color::TOOLBAR ), shadeColor( Color::SHADE1 )
{
	this->setPadding( 0 );
	this->layout = layout;
	switch ( layout )
	{
	case HORIZONTAL:
		this->setLayout( new SidewaysLayout() );
		break;
	case VERTICAL:
		this->setLayout( new StackedLayout() );
	}
	this->active = false;
}

MenuGroup::~MenuGroup()
{}

//----------------------------------------------------------------------------------------
//	public virtual methods (EventHandler)
//----------------------------------------------------------------------------------------

void
MenuGroup::deliverEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	this->Container::deliverEvent( anEvent );

	switch ( type )
	{
	case Event::CHANGE_EVENT:
		this->setRedrawAll( true );
		this->forwardRedrawAllChangeEvent();
		break;
	case Event::ACTION_EVENT:
		{
			const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
			const String& command = ae.getCommand();
			if ( command.startsWith( "ACTIVE" ) )
			{
				this->active = true;
			}
			else if ( command.startsWith( "INACTIVE" ) )
			{
				this->active = false;
			}
			else if ( command.contains( "CLICKED" ) )
			{
				this->active = false;
			}
		}
	}
}

//----------------------------------------------------------------------------------------
//	public virtual methods (Component)
//----------------------------------------------------------------------------------------

void
MenuGroup::processEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	switch ( type )
	{
	case Event::MOTION_EVENT:
		break;
		if ( this->active )
		{
			const MotionEvent& me = dynamic_cast<const MotionEvent&>( anEvent );
			int x = me.getX();
			int y = me.getY();

			if ( this->contains( x, y ) )
			{
				this->activateMenuContaining( x, y );
			}
		}
	}
}

void
MenuGroup::activateMenuContaining( int x, int y )
{
	//this->active = false;
	int activated_menu = -1;
	Sequence& components = this->getComponents();
	int max = components.getLength();
	for ( int i=0; i < max; i++ )
	{
		Object& obj = components.elementAt( i );
		Menu& menu = dynamic_cast<Menu&>( obj );
		//this->active |=
		if ( menu.activateIfContains( x, y ) )
		{
			activated_menu = i;
			break;
		}
	}

	if ( -1 != activated_menu )
	{
		for ( int i=0; i < max; i++ )
		{
			Object& obj = components.elementAt( i );
			Menu& menu = dynamic_cast<Menu&>( obj );
			if ( i != activated_menu )
			{
				menu.deactivate();
			}
		}
	}
}

void
MenuGroup::setOuterShade( const Color& shade )
{
	this->shadeColor.setTo( shade );
}

bool
MenuGroup::draw( GraphicsContext& gc ) const
{
	bool redraw_all = this->isRedrawAll();
	bool draw_again = false;

	Color original_background( gc.getBackground() );

	if ( redraw_all )
	{
		Region bounds = this->getBounds();

		switch ( this->layout )
		{
		case HORIZONTAL:
			gc.setBackground( Color::TOOLBAR );
			gc.setFill( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( bounds );
			this->drawOutward3DBox( gc, bounds, Color::SHINE1, this->shadeColor );
			break;
		case VERTICAL:
		default:
			gc.setBackground( Color::TOOLBAR );
			gc.setFill( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( bounds );
			gc.setForeground( Color::SHADOW_GREY );
			this->drawOutward3DBox( gc, bounds, Color::SHINE1, this->shadeColor );
			//gc.drawRectangleRegion( bounds );
		}
	}

	this->Container::draw( gc );

	this->setChanged( false );
	this->setRedrawAll( false );
	gc.setBackground( original_background );

	return draw_again;
}

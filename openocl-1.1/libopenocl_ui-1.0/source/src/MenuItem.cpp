/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/MenuItem.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

unsigned int MenuItem::padding = 0;

MenuItem::MenuItem( const String& name ) : Selector( name )
{
	this->text   = new Region();

	this->setEventMask( Event::MOUSE_EVENT );
	this->setMargin( 5 );
	this->setIndent( 10 );

	this->valid = false;
	this->pressed = false;
}

MenuItem::MenuItem( const String& name, const String& command ) : Selector( name, command )
{
	this->text   = new Region();

	this->setEventMask( Event::MOUSE_EVENT );

	this->setMargin( 5 );
	this->setIndent( 10 );

	this->valid = false;
	this->pressed = false;
}

MenuItem::MenuItem( const String& name, const String& command, const String& label )
: Selector( name, command, label )
{
	this->setData( label );
	this->text   = new Region();

	this->setEventMask( Event::MOUSE_EVENT );

	this->setMargin( 5 );
	this->setIndent( 10 );

	this->valid = false;
	this->pressed = false;
}

MenuItem::~MenuItem()
{
	delete this->text;
}

void
MenuItem::processEvent( const Event& anEvent )
{
	switch ( anEvent.getType() )
	{
	case Event::MOUSE_EVENT:
#ifdef DEBUG_OPENOCL_UI_MENUITEM
		fprintf( stderr, "MenuItem::processEvent( MouseEvent& )\n" );
#endif
		const MouseEvent& event = dynamic_cast<const MouseEvent&>( anEvent );
		if ( this->contains( event.getX(), event.getY() ) )
		{
			flags button = event.getButton();
			if ( MouseEvent::BUTTON1_PRESS == (MouseEvent::BUTTON1_PRESS & button))
			{
				this->pressed = true;
			}
			else if ( this->pressed & (MouseEvent::BUTTON1_RELEASE == (MouseEvent::BUTTON1_RELEASE & button)) )
			{
				this->pressed = false;
				this->fireSelectorEvent( "openocl::ui::MenuItem.CLICKED" );
			}
		} else {
			this->pressed = false;
		}
		break;
	}
}

void
MenuItem::doLayout( const GraphicsContext& gc )
{
	unsigned int margin  = this->getMargin();
	unsigned int indent  = this->getIndent();
	unsigned int padding = this->padding;

	Region bounds = this->getBounds();
	Region text;

	TextExtents* extents = gc.measureTextExtents( this->getLabel() );

	text.width = extents->width;
	text.height = extents->ascent;
	text.x1 = bounds.x1 + padding + margin + indent;
	text.x2 = text.x1 + text.width - 1;
	text.y1 = bounds.y1 + padding + margin;
	text.y2 = text.y1 + text.height - 1;

	this->valid = true;
	*this->text = text;

	delete extents;
}

Dimensions
MenuItem::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	unsigned int margin = this->getMargin();
	unsigned int indent = this->getIndent();
	
	TextExtents* extents = gc.measureTextExtents( this->getLabel() );
	dim.width  = extents->width  + padding*2 + margin*4 + indent;
	dim.height = extents->ascent + extents->descent + padding*2 + margin*2;	
	delete extents;
	
	return dim;
}

bool
MenuItem::draw( GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	Region text   = *this->text;

	if ( this->hasChanged() )
	{
		gc.setForeground( Color::BLACK );
		gc.drawBoundedString( this->getLabel(), text );
		this->setChanged( false );
	}

	return false;
}

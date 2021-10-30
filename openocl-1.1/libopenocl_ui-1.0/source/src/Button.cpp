/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Button.h"
#include "openocl/ui/FontManager.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/Textual.h"

#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
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

unsigned int Button::padding = 5;

Button::Button( const String& name ) : Selector( name )
{
	this->init();
}

Button::Button( const String& name, const String& action ) : Selector( name, action )
{
	this->init();
}

Button::Button( const String& name, const String& action, const String& label ) : Selector( name, action, label )
{
	this->setData( label );
	this->init();
}

void
Button::init()
{
	this->button = new Region();
	this->text   = new Region();

	this->image = null;
	this->setEventMask( Event::MOUSE_EVENT );
	this->setIndent( 15 );
	this->setMargin( 10 );
	this->valid = false;
	this->pressed = false;
}

Button::~Button()
{
	delete this->button;
	delete this->text;
}

/*
 *	Components processEvents to determine whether or not
 *	the event affects it. In the case of a button if the
 *	event occurred within the bounds of the button it
 *	should fire off an Action Event.
 */
void
Button::processEvent( const Event& anEvent )
{
	bool fire_change = false;
	
	flags type = anEvent.getType();
	if ( Event::POINTER_EVENT == (Event::POINTER_EVENT & type) )
	{
		const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
		if ( Event::MOTION_EVENT == type )
		{
			if ( (false == this->contains( pe.getX(), pe.getY() )) && this->pressed )
			{
				this->pressed = false;
				fire_change = true;
			}
		}
		else if ( Event::MOUSE_EVENT == (Event::MOUSE_EVENT & type) )
		{
			//fprintf( stderr, "Button::processEvent( MouseEvent& )\n" );
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
					this->fireSelectorEvent( "openocl::ui::Button.PRESSED" );
					this->pressed = false;
				}
				if ( MouseEvent::BUTTON2_PRESS == (MouseEvent::BUTTON2_PRESS & button))
				{
					this->pressed = true;
				}
				else if ( this->pressed & (MouseEvent::BUTTON2_RELEASE == (MouseEvent::BUTTON2_RELEASE & button)) )
				{
					this->fireSelectorEvent( "openocl::ui::Button.PRESSED" );
					this->pressed = false;
				}
				if ( MouseEvent::BUTTON3_PRESS == (MouseEvent::BUTTON3_PRESS & button))
				{
					this->pressed = true;
				}
				else if ( this->pressed & (MouseEvent::BUTTON3_RELEASE == (MouseEvent::BUTTON3_RELEASE & button)) )
				{
					this->fireSelectorEvent( "openocl::ui::Button.PRESSED" );
					this->pressed = false;
				}
				fire_change = true;
			} else {
				this->pressed = false;
			}
		}

		if ( fire_change )
		{
			this->fireChangeEvent();
			const_cast<Panel&>( this->getContainingPanel() ).redraw();
		}
	}
}

void
Button::doLayout( const GraphicsContext& gc )
{
	gc.setFont( gc.getFontManager().getDefaultFont() );

	flags hints = this->getTextLayoutHints();
	unsigned int indent  = this->getIndent();
	unsigned int margin  = this->getMargin();

	Region bounds = this->getBounds();
	Region button = bounds;
	Region text;

	button.x1++;
	button.x2--;
	button.y1++;
	button.y2--;
	button.width -= 2;
	button.height -= 2;


#ifdef DEBUG_OPENOCL_UI_BUTTON
	fprintf( stderr, "Button::doLayout: width: %3i height %3i\n", bounds.width, bounds.height );
#endif

	int cx = bounds.x1 + bounds.width/2;
	int cy = bounds.y1 + bounds.height/2;

	TextExtents* extents = gc.measureTextExtents( this->getData() );
	text.width  = extents->width;
	text.height = extents->ascent + extents->descent;

	//text.width  = textWidth + (margin * 2) + (indent * 2);//(Button::padding*2);
	//text.height = textHeight + (margin * 2);//(Button::padding*2);

	switch ( Textual::ALIGN & hints )
	{
	case Textual::LEFT:
		text.x1 = button.x1 + margin + indent;
		text.x2 = text.x1 + text.width - 1;
	case Textual::CENTERED:
		//	Intentional fall-through
	default:
		text.x1 = cx - text.width/2;
		text.x2 = text.x1 + text.width - 1;
	}

	switch ( Textual::VALIGN & hints )
	{
	//Textual::MIDDLE:
		//	Intentional fall-through
	default:
		text.y1 = cy - text.height/2;
		text.y2 = text.y1 + text.height - 1;
	}

	this->valid = true;
	*this->button = button;
	*this->text = text;

	delete extents;
}

Dimensions
Button::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	unsigned int margin = this->getMargin();
	unsigned int indent = this->getIndent();

	gc.setFont( gc.getFontManager().getDefaultFont() );

	TextExtents* extents = gc.measureTextExtents( this->getData() );
	dim.width = extents->width + margin*2 + indent*2;
	dim.height = extents->ascent + extents->descent + (margin*2);
	delete extents;
	
	return dim;
}

bool
Button::draw( GraphicsContext& gc ) const
{
	gc.setFont( gc.getFontManager().getDefaultFont() );

	//IO::err().printf( "Button::draw()\n" );
	//this->Component::draw( gc );

	Region bounds = this->getBounds();
	Region button = *this->button;
	Region text = *this->text;

	gc.setFill( Color::TOOLBAR );
	gc.drawFilledRectangleRegion( button );

	if ( this->hasFocus() )
	{
		gc.setForeground( Color::SHADOW_GREY );
		gc.drawRectangleRegion( bounds );
	} else {
		this->drawInward3DBox( gc, bounds, Color::SHINE1, Color::SHADE1 );
	}
	
	if ( this->pressed )
	{
		this->drawInward3DBox( gc, button, Color::SHINE2, Color::SHADE2 );

		gc.setForeground( Color::DARK_GREY );
		gc.setBackground( Color::TOOLBAR );
		gc.drawString( this->getData(), text.x1 + 1, text.y2 + 1 );
	} else {
		this->drawOutward3DBox( gc, button, Color::SHINE1, Color::SHADE2 );

		gc.setForeground( Color::DARK_GREY );
		gc.setBackground( Color::TOOLBAR );
		gc.drawString( this->getData(), text.x1, text.y2 );
	}

#ifdef DEBUG_OPENOCL_UI_BUTTON
	gc.setForeground( Color::RED );
	gc.drawRectangleRegion( *this->button );
	gc.setForeground( Color::GREEN );
	gc.drawRectangleRegion( *this->text );
#endif
	return false;
}

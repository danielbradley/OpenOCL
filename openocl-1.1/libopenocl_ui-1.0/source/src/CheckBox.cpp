/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/CheckBox.h"
#include "openocl/ui/FontManager.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"

#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
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

unsigned int CheckBox::size    = 10;

CheckBox::CheckBox( const String& name )
: Selector( name )
{
	this->init();
}

CheckBox::CheckBox( const String& name, const String& action )
: Selector( name, action )
{
	this->init();
}

CheckBox::CheckBox( const String& name, const String& action, const String& label )
: Selector( name, action, label )
{
	this->setData( label );
	this->init();
}

void
CheckBox::init()
{
	this->outline = new Region();
	this->margin  = new Region();
	this->text    = new Region();

	this->setEventMask( Event::MOUSE_EVENT );
	this->setPadding( 10 );
	this->setIndent( 15 );
	this->setMargin( 5 );

	this->valid    = false;
	this->pressed  = false;
	this->selected = false;

	this->checked   = new String( "CHECKED" );
	this->unchecked = new String( "UNCHECKED" );

	this->hints = LayoutManager::LEFT | LayoutManager::MIDDLE;
}

CheckBox::~CheckBox()
{
	delete this->outline;
	delete this->margin;
	delete this->text;

	delete this->checked;
	delete this->unchecked;
}

/*
 *	Components processEvents to determine whether or not
 *	the event affects it. In the case of a CheckBox if the
 *	event occurred within the bounds of the button it
 *	should fire off an Action Event.
 */
void
CheckBox::processEvent( const Event& anEvent )
{
	bool changed = false;

	switch ( anEvent.getType() )
	{
	case Event::MOUSE_EVENT:
		{
			const MouseEvent& event = dynamic_cast<const MouseEvent&>( anEvent );
			if ( this->outline->contains( event.getX(), event.getY() ) )
			{
				flags button = event.getButton();
				if ( MouseEvent::BUTTON1_PRESS == (MouseEvent::BUTTON1_PRESS & button))
				{
					this->pressed = true;
					changed = true;

					//	It actually might be more intuitive if Check Boxes only
					//	gained focus when "tabbed to", or not.
					//
					//if ( true )//!this->hasFocus() )
					//{
					//	FocusEvent request( *this, 0, FocusEvent::FOCUS_REQUESTED );
					//	this->fireEvent( request );
					//	changed = true;
					//}
				}
				else if ( this->pressed && (MouseEvent::BUTTON1_RELEASE == (MouseEvent::BUTTON1_RELEASE & button)) )
				{
					this->selected = !this->selected;
					if ( this->selected )
					{
						this->fireSelectorEvent( "openocl::ui::CheckBox.CHECKED" );
						this->setData( "CHECKED" );
					} else {
						this->fireSelectorEvent( "openocl::ui::CheckBox.UNCHECKED" );
						this->setData( "UNCHECKED" );
					}
					this->pressed = false;
					changed = true;
				}
			} else {
				this->pressed = false;
			}
		}
		break;
	}
	
	if ( changed )
	{
		this->fireChangeEvent();
		this->getContainingPanel().redraw();
	}
}

void
CheckBox::doLayout( const GraphicsContext& gc )
{
	unsigned int padding = this->getPadding();
	unsigned int m = this->getMargin();

	int outline_size = this->size + 2*m;
	//int margin_size = this->size;

	Region bounds = this->getBounds();
	Region outline;
	Region margin;

	int cx = bounds.x1 + bounds.width/2;
	int cy = bounds.y1 + bounds.height/2;

	switch ( (this->hints & LayoutManager::ALIGN) )
	{
	case LayoutManager::LEFT:
		outline.x1 = bounds.x1 + padding;
		outline.width = outline_size;
		outline.x2 = outline.x1 + outline.width - 1;
		break;
	case LayoutManager::RIGHT:
		outline.x2 = bounds.x2 - padding;
		outline.width = outline_size;
		outline.x1 = outline.x2 - outline.width + 1;
		break;
	case LayoutManager::CENTERED:
		outline.x1 = cx - outline_size/2;
		outline.width = outline_size;
		outline.x2 = outline.x1 + outline.width - 1;
	}

	switch ( (this->hints & LayoutManager::VALIGN) )
	{
	case LayoutManager::TOP:
		outline.y1 = bounds.y1 + padding;
		outline.height = outline_size;
		outline.y2 = outline.y1 + outline.height - 1;
		break;
	case LayoutManager::BOTTOM:
		outline.y2 = bounds.y2 - padding;
		outline.height = outline_size;
		outline.y1 = outline.y2 + outline.height - 1;
		break;
	case LayoutManager::MIDDLE:
		outline.y1 = cy - outline_size/2;
		outline.height = outline_size;
		outline.y2 = outline.y1 + outline.height - 1;
	}
	
	margin = outline.shrunkBy( m );

	*this->outline = outline;
	*this->margin  = margin;

/*
	gc.setFont( gc.getFontManager().getDefaultFont() );

	flags hints = this->getLayoutHints();
	unsigned int indent  = this->getIndent();
	unsigned int margin  = this->getMargin();

	Region bounds = this->getBounds();
	Region CheckBox;
	Region text;

	TextExtents* extents = gc.measureTextExtents( this->getData() );
	this->textWidth = extents->width;
	this->textHeight = extents->ascent + extents->descent;
	button.width  = textWidth + (margin * 2) + (indent * 2);//(Button::padding*2);
	button.height = textHeight + (margin * 2);//(Button::padding*2);

	switch ( ALIGN & hints )
	{
	case LEFT:
		button.x1 = bounds.x1;
		button.x2 = button.x1 + button.width - 1;
		break;
	case CENTERED:
		button.x1 = (bounds.x1 + (bounds.width/2)) - (button.width/2);
		button.x2 = button.x1 + button.width - 1;
		break;
	case RIGHT:
		button.x2 = bounds.x2;
		button.x1 = button.x2 - (button.width - 1);
		break;
	default:
		button.x1 = bounds.x1;
		button.x2 = bounds.x2;
		button.width = (button.x2 - button.x1) + 1;
	}

	switch ( VALIGN & hints )
	{
	case TOP:
		button.y1 = bounds.y1;
		button.y2 = button.y1 + button.height - 1;
		break;
	case MIDDLE:
		button.y1 = (bounds.y1 + (bounds.height/2)) - (button.height/2);
		button.y2 = button.y1 + button.height - 1;
		break;
	case BOTTOM:
		button.y2 = bounds.y2;
		button.y1 = button.y2 - (button.height - 1);
		break;
	default:
		button.y1 = bounds.y1;
		button.y2 = bounds.y2;
		button.height = (button.y2 - button.y1) + 1;
	}

	text.width = extents->width;
	text.height = extents->ascent;
	text.x1 = button.x1 + (button.width/2) - (text.width/2);
	text.x2 = text.x1 + text.width - 1;
	text.y1 = button.y1 + margin;
	text.y2 = text.y1 + extents->ascent;
	//text.y1 = text.y2 - text.height + 1;

	this->valid = true;
	*this->button = button;
	*this->text = text;

	delete extents;
*/
}

Dimensions
CheckBox::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;

	unsigned int padding = this->getPadding();
	unsigned int margin = this->getMargin();

//	gc.setFont( gc.getFontManager().getDefaultFont() );

//	TextExtents* extents = gc.measureTextExtents( this->getData() );
//	dim.width = extents->width + margin*2 + indent*2;
//	dim.height = extents->ascent + extents->descent + (margin*2);
//	delete extents;

	dim.width  = this->size + margin*2 + padding*2;
	dim.height = this->size + margin*2 + padding*2;
	
	return dim;
}

bool
CheckBox::draw( GraphicsContext& gc ) const
{
	Region outline = *this->outline;
	Region margin  = *this->margin;

	gc.setFill( Color::CANVAS );
	gc.drawFilledRectangleRegion( outline );

	if ( this->selected )
	{
		gc.setFill( Color::BLACK );
		gc.drawFilledRectangleRegion( margin );
	}

	if ( this->hasFocus() )
	{
		gc.setForeground( Color::SHADOW_GREY );
		gc.drawRectangleRegion( outline );
	} else {
		this->drawInward3DBox( gc, outline, Color::SHINY, Color::SHADE );
	}

	return false;
}


void
CheckBox::setLayoutHints( flags layoutHints )
{
	this->hints = layoutHints;
}

const String&
CheckBox::getState() const
{
	if ( this->isChecked() )
	{
		return *this->checked;
	} else {
		return *this->unchecked;
	}
}

bool
CheckBox::isChecked() const
{
	return this->selected;
}




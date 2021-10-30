/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/NumberField.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Panel.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

NumberField::NumberField( const String& name ) : TextField( name )
{
	this->setTextLayoutHints( Textual::CENTERED );
	this->decPressed = false;
	this->incPressed = false;
	this->hover      = false;

	this->asFloat = false;
	this->min   = 0.0;
	this->value = 0.0;
	this->max   = 0xFFFFFFFF;
	this->inc   = 1.0;
}

NumberField::~NumberField()
{}


//
//
//

void
NumberField::processEvent( const Event& anEvent )
{
	bool fire_change = false;

	unsigned int type = anEvent.getType();
	this->TextField::processEvent( anEvent );

	switch ( type )
	{
	case Event::MOTION_EVENT:
		{
			const MotionEvent& mo = dynamic_cast<const MotionEvent&>( anEvent );
			int x = mo.getX();
			int y = mo.getY();
			if ( this->contains( x, y ) )
			{
				if ( !this->hover )
				{
					this->hover = true;
					fire_change = true;
				}
			} else {
				if ( this->hover )
				{
					this->hover = false;
					fire_change = true;
				}
			}
		}
		break;
	case Event::MOUSE_EVENT:
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			flags button = me.getButton();
			int x = me.getX();
			int y = me.getY();
			if ( me.BUTTON1_PRESS == (me.BUTTON1_PRESS & button ) )
			{
				if ( this->decreaseRegion.contains( x, y ) )
				{
					this->decPressed = true;
					fire_change = true;
				}
				else if ( this->increaseRegion.contains( x, y ) )
				{
					this->incPressed = true;
					fire_change = true;
				}
			}
			else if ( me.BUTTON1_RELEASE == (me.BUTTON1_RELEASE & button ) )
			{
				if ( this->decreaseRegion.contains( x, y ) )
				{
					this->decrementValue();
					this->decPressed = false;
					fire_change = true;
				}
				else if ( this->increaseRegion.contains( x, y ) )
				{
					this->incrementValue();
					this->incPressed = false;
					fire_change = true;
				}
			}
			else 
			{
				this->decPressed = false;
				this->incPressed = false;
				fire_change = true;

			}
		}
		break;
	case Event::FOCUS_EVENT:
		const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
		if ( FocusEvent::LOSE_FOCUS == fe.getFocusEventType() )
		{
			const String& data = this->getData();
			double new_value = data.getValue();
			if ( this->value != new_value )
			{
				this->setValue( new_value );
			}
			fire_change = true;
		}
	}

	if ( fire_change )
	{
		this->fireChangeEvent();
		const_cast<Panel&>( this->getContainingPanel() ).relayout();
	}
}

void
NumberField::doLayout( const GraphicsContext& gc )
{
	this->TextField::doLayout( gc );
	
	Region edge    = this->getEdgeRegion();
	Region content = this->getContentRegion();
	Region text    = this->getTextRegion();

	this->decreaseRegion.y1 = edge.y1 + 1;
	this->decreaseRegion.y2 = edge.y2 - 1;
	this->decreaseRegion.height = edge.height - 2;

	this->decreaseRegion.x1 = edge.x1 + 1;
	this->decreaseRegion.width = this->decreaseRegion.height;
	this->decreaseRegion.x2 = this->decreaseRegion.x1 + this->decreaseRegion.width - 1;

	this->increaseRegion.y1 = edge.y1 + 1;
	this->increaseRegion.y2 = edge.y2 - 1;
	this->increaseRegion.height = edge.height - 2;

	this->increaseRegion.x2 = edge.x2 - 1;
	this->increaseRegion.width = this->increaseRegion.height;
	this->increaseRegion.x1 = this->increaseRegion.x2 - this->increaseRegion.width + 1;
}

//--------------------------------------------------------------------------
//	public virtual methods (TextField)
//--------------------------------------------------------------------------

void
NumberField::setData( const String& aString )
{
	double value = aString.getValue();
	this->setValue( value );
}

//--------------------------------------------------------------------------
//	public methods
//--------------------------------------------------------------------------

void
NumberField::setMinValue( double aValue )
{
	this->min = aValue;
}

void
NumberField::setValue( double aValue )
{
	this->value = aValue;
	if ( this->value < this->min ) this->value = min;
	if ( this->value > this->max ) this->value = max;

	if ( this->asFloat )
	{
		FormattedString str( "%.2f", this->value );
		this->TextField::setData( str );
	}
	else
	{
		int int_value = (int) this->value;
		FormattedString str( "%i", int_value );
		this->TextField::setData( str );
	}
}

void
NumberField::setMaxValue( double aValue )
{
	this->max = aValue;
}

void
NumberField::setIncrement( double aValue )
{
	this->inc = aValue;
}

void
NumberField::setAsFloat( bool asFloat )
{
	this->asFloat = asFloat;
	if ( !asFloat )
	{
		this->inc = 1.0;
	}
}

//--------------------------------------------------------------------------
//	public virtual constant methods
//--------------------------------------------------------------------------

bool
NumberField::draw( GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	Region box    = this->getEdgeRegion();
	Region text   = this->getTextRegion();
	//Region hint   = *this->hint;

	int min_width  = (this->getMargin()*2) + (this->getIndent() * 2) + 10;
	int min_height = 15;
	
	if ( this->hasChanged() )
	{
		if ( (bounds.width > min_width) && (bounds.height > min_height) )
		{
			Color previous_background( gc.getBackground().getColor() );
			Color* background = null;
			if ( this->isEditable() )
			{
				background = new Color( Color::CANVAS );
			} else {
				background = new Color( Color::SCROLLBAR );
			}

			//	Draw canvas background
			gc.setFill( *background );
			gc.drawFilledRectangle( box.x1, box.y1, box.width, box.height );

			if ( this->hasFocus() )
			{
				gc.setForeground( Color::SHADOW_GREY );
				gc.drawRectangleRegion( box );
			} else {
				this->drawInward3DBox( gc, box, Color::SHINY, Color::SHADE );
			}
			
			const String& str = this->getText();
			if ( this->hover )
			{
				gc.setFill( Color::DIALOG );
				gc.drawFilledRectangleRegion( this->decreaseRegion );
				gc.drawFilledRectangleRegion( this->increaseRegion );
				if ( this->decPressed )
				{
					this->drawInward3DBox( gc, this->decreaseRegion, Color::SHINY, Color::SHADE );
				}
				else
				{
					this->drawOutward3DBox( gc, this->decreaseRegion, Color::SHINY, Color::SHADE );
				}
				
				if ( this->incPressed )
				{
					this->drawInward3DBox( gc, this->increaseRegion, Color::SHINY, Color::SHADE );
				}
				else
				{
					this->drawOutward3DBox( gc, this->increaseRegion, Color::SHINY, Color::SHADE );
				}
				
				gc.setForeground( Color::TEXT );
				gc.setBackground( *background );
				gc.drawBoundedString( str, text, 0 );
			} else {
				gc.setForeground( Color::INACTIVE_TEXT );
				gc.setBackground( *background );
				gc.drawBoundedString( str, text, 0 );
			}

			gc.setBackground( previous_background );
			delete background;
		}
		this->setChanged( false );
	}
	return false;
}

//--------------------------------------------------------------------------
//	private methods
//--------------------------------------------------------------------------

void
NumberField::decrementValue()
{
	double val = this->value - this->inc;
	this->setValue( val );
}

void
NumberField::incrementValue()
{
	double val = this->value + this->inc;
	this->setValue( val );
}

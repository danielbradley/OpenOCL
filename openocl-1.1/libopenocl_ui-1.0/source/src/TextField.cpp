/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/FocusManager.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/TextField.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/Math.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/KeyEvent.h>
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

TextField::TextField( const String& name ) : Textual( name )
{
	this->editable = true;

	this->setEdge( 10 );
	this->setMargin( 2 );
	this->setBorder( 0 );
	this->setPadding( 2 );

	this->setEventMask( Event::MOUSE_EVENT | Event::KEY_EVENT );

	this->setTextLayoutHints( Textual::MIDDLE | Textual::LEFT );
	this->setSpacing( 0 );
	this->setIndent( 10 );
	this->setRightIndent( 10 );
	
	this->value = new StringBuffer();
	this->value->append( "" );
	this->hintValue = new String();
//	this->active = false;

	this->hint   = new Region();

	this->preferredWidth = 0;


	this->clickPoint = -1;
	this->cursorPosition = 0;
}

TextField::~TextField()
{
	delete this->value;
	delete this->hintValue;
	delete this->hint;
}

void
TextField::processEvent( const Event& anEvent )
{
	//fprintf( stderr, "TextField::processEvent()\n" );

	bool fire_change = false;
	bool fire_event  = false;
	int x;
	int y;

	unsigned int type = anEvent.getType();

	switch ( type )
	{
	//case Event::FOCUS_EVENT:
	//	{
	//		const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
	//		switch ( fe.getFocusEventType() )
	//		{
	//		case FocusEvent::CIRCULATE_FORWARD:
	//		case FocusEvent::CIRCULATE_BACKWARD:
	//		case FocusEvent::TAKE_FOCUS:
	//			this->active = true;
	//			break;
	//		case FocusEvent::LOSE_FOCUS:
	//			this->active = false;
	//		}
	//		fire_change = true;
	//	}		
	//	break;
	case Event::MOUSE_EVENT:
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			flags button = me.getButton();
			if ( this->contains( (x = me.getX()), (y = me.getY()) ) )
			{
				if ( me.BUTTON1_PRESS == (me.BUTTON1_PRESS & button ) )
				{
					FocusEvent ae( *this, 0, FocusEvent::FOCUS_REQUESTED );
					this->fireEvent( ae );

#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
					fprintf( stderr, "%s.TextField::processEvent: mouse click: %i\n", this->getName().getChars(), x );
#endif
					//this->clickPoint = x - this->getBounds().x1;
					this->clickPoint = x;
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
					fprintf( stderr, "%s.TextField::processEvent: click point: %i\n", this->getName().getChars(), this->clickPoint );
#endif
					//this->active = true;
				}
				//fire_change = true;
			} else {
				//this->active = false;
				//fire_change = true;
			}
		}
		break;
	case Event::KEYDOWN_EVENT:
		if ( this->hasFocus() && this->editable )
		{
			delete this->hintValue;
			this->hintValue = new String();
		
			const KeyEvent& ke = dynamic_cast<const KeyEvent&>( anEvent );
			unsigned int key = ke.getKey();
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
			fprintf( stderr, "TextField::processEvent( KEY_EVENT ): %i\n", key );
#endif

			if ( KeyEvent::PRINTABLE_MASK & key )
			{
				switch( key )
				{
				case KeyEvent::BACKSPACE:
					if ( 0 < this->cursorPosition )
					{
						this->cursorPosition--;
						this->value->removeCharAt( this->cursorPosition );
					}
					break;
				case KeyEvent::DELETE:
					if ( this->cursorPosition < this->value->getLength() )
					{
						this->value->removeCharAt( this->cursorPosition );
					}
					break;
				case KeyEvent::RETURN:
					//XXX-Debug this->loseFocus();
					fire_event = true;
					break;
//				case '\t':
//					{
//						FocusEvent focus_event( *this, 0, FocusEvent::CIRCULATE_FORWARD );
//						this->fireEvent( focus_event );
//						fprintf( stderr, "TextField::processEvent: TAB\n" );
//					}
				default:
					this->value->insertCharAt( this->cursorPosition, key );
					this->cursorPosition++;
				}
			} else {
				switch( key )
				{
				case KeyEvent::LEFT:
					this->cursorPosition--;
					break;
				case KeyEvent::RIGHT:
					this->cursorPosition++;
					break;
				}
			}

			cursorPosition = Math::between( this->cursorPosition, 0, this->value->getLength() );
			
			fire_change = true;
		}
	}

	if ( fire_change )
	{
		this->fireChangeEvent();
		const_cast<Panel&>( this->getContainingPanel() ).relayout();	//	To resize text region
	}
	
	if ( fire_event )
	{
		FormattedString str( "%s.openocl::ui::TextField.ENTER", this->getName().getChars() );
		this->fireEvent( ActionEvent( *this, null, str, this->value->asString() ) );
	}
}

/*
void
TextField::doLayout( const GraphicsContext& gc )
{
	this->Textual::doLayout( gc );

	Region text = this->getTextRegion();
	Region hint;

	TextExtents* hint_extents = gc.measureTextExtents( *this->hintValue );

	hint.x1 = text.x2 + 6;
	hint.width = hint_extents->width;
	hint.x2 = hint.x1 + hint.width - 1;
	hint.y1 = text.y1;
	hint.y2 = text.y2;
	hint.height = text.height;

	delete hint_extents;

	*this->hint = hint;
}
*/

void
TextField::setData( const String& aString )
{
	delete this->value;
	this->value = new StringBuffer();
	this->value->append( aString );
	this->cursorPosition = this->value->getLength();
	try
	{
		this->fireChangeEvent();
		this->getContainingPanel().redraw();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
}

void
TextField::setHint( const String& aHint )
{
	delete this->hintValue;
	this->hintValue = new String( aHint );
}

void
TextField::setPreferredWidth( unsigned int aWidth )
{
	this->preferredWidth = aWidth;
}

//---------------------------------------------------------------------------------------
//	public methods
//---------------------------------------------------------------------------------------

void
TextField::setEditable( bool editable )
{
	this->editable = editable;
}

bool
TextField::draw( GraphicsContext& gc ) const
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
			if ( editable )
			{
				background = new Color( Color::CANVAS );
			} else {
				background = new Color( Color::SCROLLBAR );
			}

			//	Draw canvas background
			gc.setFill( *background );
			gc.drawFilledRectangle( box.x1, box.y1, box.width, box.height );

			this->drawInward3DBox( gc, box, Color::SHINY, Color::SHADE );
	
			const String& str = this->value->asString();
			if ( this->hasFocus() && this->editable )
			{
				gc.setForeground( Color::BLACK );
			
				//	If this->clickPoint >= 0 then it indicates that the mouse
				//	has been reclicked in the window so we need to recalculate where
				//	the cursor position should be.
				//
				//
				//	           	*      --------click point
				//	     |                         ----- text.x1
				//	     | --delta--|
				//            The text of the text field
				//


				if ( 0 <= this->clickPoint )
				{
					int len = str.getLength();
					int delta = this->clickPoint - text.x1;
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
					fprintf( stderr, "TextField::draw: text.x1 %i\n", text.x1 );
					fprintf( stderr, "TextField::draw: delta   %i\n", delta );
#endif
					String* tmp;
					int tmp_len;

					//
					//	Try increasing lengths of the string and compare the length
					//	against the delta. If the length is greater than the delta
					//	choose the cursor position of that space.
				
					for ( int i=0; i < len; i++ )
					{
						try
						{
							tmp = str.substring( 0, i );
							tmp_len = (int) gc.measureTextWidth( *tmp );
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
							fprintf( stderr, "TextField::draw: tmp_len: %i\n", tmp_len );
#endif						
							if ( delta < 0 )
							{
								const_cast<TextField*>( this )->cursorPosition = 0;
								i = len;
							}
							else if ( delta < tmp_len )
							{
								const_cast<TextField*>( this )->cursorPosition = i + 1;
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
								fprintf( stderr, "TextField::draw: i: %i\n", this->cursorPosition );
#endif
								i = len;
							} else {
								const_cast<TextField*>( this )->cursorPosition = i + 1;
							}							
							
							delete tmp;
						} catch ( IndexOutOfBoundsException* ex ) {
							delete ex;
						}
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD_DRAW
						fprintf( stderr, "TextField::draw: dropping out of for loop\n" );
#endif
					}
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD_DRAW
					fprintf( stderr, "TextField::draw: break out of for loop\n" );
#endif
					const_cast<TextField*>( this )->clickPoint = -1;
				}

				int cursor_offset;
				if ( 0 == this->cursorPosition )
				{
					cursor_offset = text.x1 - 1;
				}
				else
				{
					String* start;
					try
					{
						start = str.substring( 0, this->cursorPosition - 1 );
					} catch ( IndexOutOfBoundsException* ex ) {
						delete ex;
						start = new String();
					}
					cursor_offset = text.x1 + gc.measureTextWidth( *start ) - 1;
					delete start;
				}
			
				gc.setForeground( Color::SHADOW_GREY );
				gc.drawRectangleRegion( box );
				
				gc.setForeground( Color::TEXT );
				gc.setBackground( *background );
				gc.drawBoundedString( str, text, 0 );
				//gc.drawString( str, text.x1, text.y2 );
				gc.drawLine( cursor_offset + 2, text.y1, cursor_offset + 2, text.y2 );
			} else {
				gc.setForeground( Color::INACTIVE_TEXT );
				gc.setBackground( *background );
				gc.drawBoundedString( str, text, 0 );
				//gc.drawString( str, text.x1, text.y2 );
			}
			//gc.drawBoundedString( str, text, 0 );
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
			this->Textual::draw( gc );
#endif
			gc.setBackground( previous_background );
			delete background;
		}
	
		//	Indicate to containing container that this element
		//	doesn't need to be repainted until it is moved.

#ifdef DEBUG_OPENOCL_UI_TEXTUAL
		this->Textual::draw( gc );
#endif
		this->setChanged( false );
	}
	return false;
}

Dimensions
TextField::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD_GETPREFERREDDIMENSIONS
	fprintf( stderr, "TextField::getPreferredDimensions( gc, %i, %i )\n", width, height );
#endif

	Dimensions d = this->Textual::getPreferredDimensions( gc, width, height );
	
	if ( 0 < this->preferredWidth )
	{
		d.width = this->preferredWidth;
	}
	else if ( (d.width < width) && (50 < width) )
	{
		d.width = Math::max( d.width, 50 );
	}

	d.width  = Math::min( width, d.width );

	return d;
}

const String&
TextField::getData() const
{
	return this->value->asString();
}

const String&
TextField::getHint() const
{
	return *this->hintValue;
}

const String&
TextField::getText() const
{
	return this->value->asString();
}

bool
TextField::isEditable() const
{
	return this->editable;
}


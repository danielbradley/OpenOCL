/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ScrollBar.h"

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/GreyScale.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

unsigned int ScrollBar::bredth = 20;
unsigned int ScrollBar::minimumSliderLength = 10;
unsigned int ScrollBar::padding = 2;

ScrollBar::ScrollBar( int aType ) : Component( "scrollbar" )
{
	this->type = aType;

	this->scrollBar      = new Region();
	this->outline        = new Region();
	this->sliderTrough   = new Region();
	this->slider         = new Region();
	this->decreaseButton = new Region();
	this->increaseButton = new Region();

	this->lessArrow = null;
	this->moreArrow = null;
	this->thumbTab  = null;

	this->barLength = 1;
	this->sliderBeginOffset = ScrollBar::bredth + 1;
	this->sliderEndOffset = 1;
	this->sliderLength = 1;
	
	this->scrollSize = 1;
	this->viewableSize = 1;

	this->maxOffset = 1;
	this->view2scroll = 0.1;
	this->slider2scroll = 0.1;
	this->scroll2slider = 0.1;
	this->tabSize = 1;
	this->maxPixelOffset = 1;
	
	this->offset = 0;
	this->pixelOffset = 0;

	this->increment = 1;

	this->decPressed = false;
	this->incPressed = false;

	this->clickX = 0;
	this->clickY = 0;
}

ScrollBar::~ScrollBar()
{
	delete this->scrollBar;
	delete this->outline;
	delete this->sliderTrough;
	delete this->slider;
	delete this->decreaseButton;
	delete this->increaseButton;
}

void
ScrollBar::doLayout( const GraphicsContext& gc )
{
	Region scroll_bar;
	Region outline;
	Region trough;
	Region slider;
	Region dec_button;
	Region inc_button;

	scroll_bar.x1 = this->getX();
	scroll_bar.y1 = this->getY();
	scroll_bar.width = this->getWidth();
	scroll_bar.height = this->getHeight();
	scroll_bar.x2 = scroll_bar.x1 + scroll_bar.width - 1;
	scroll_bar.y2 = scroll_bar.y1 + scroll_bar.height - 1;

	switch ( this->type )
	{
	case HORIZONTAL:
		{
			outline.x1 = scroll_bar.x1;
			outline.x2 = scroll_bar.x2;
			outline.y1 = scroll_bar.y1;
			outline.y2 = scroll_bar.y2 - padding;
			outline.width = outline.x2 - outline.x1 + 1;
			outline.height = outline.y2 - outline.y1 + 1;

			dec_button.y1 = outline.y1 + 1;
			dec_button.y2 = outline.y2 - 1;
			dec_button.height = dec_button.y2 - dec_button.y1 + 1;
			dec_button.x1 = outline.x1 + 1;
			dec_button.width = dec_button.height;
			dec_button.x2 = dec_button.x1 + dec_button.width - 1;

			inc_button.y1 = dec_button.y1;
			inc_button.y2 = dec_button.y2;
			inc_button.height = dec_button.height;
			inc_button.width  = dec_button.height;
			inc_button.x2 = outline.x2 - 1;
			inc_button.x1 = inc_button.x2 - inc_button.width + 1;

			trough.x1 = dec_button.x2 + 1;
			trough.x2 = inc_button.x1 - 1;
			trough.y1 = dec_button.y1;
			trough.y2 = dec_button.y2;
			trough.width = (trough.x2 - trough.x1) + 1;
			trough.height = (trough.y2 - trough.y1) + 1;
		}
		break;
	case VERTICAL:
		{
			outline.x1 = scroll_bar.x1;
			outline.x2 = scroll_bar.x2 - padding;
			outline.y1 = scroll_bar.y1;
			outline.y2 = scroll_bar.y2;
			outline.width = outline.x2 - outline.x1 + 1;
			outline.height = outline.y2 - outline.y1 + 1;

			dec_button.x1 = outline.x1 + 1;
			dec_button.x2 = outline.x2 - 1;
			dec_button.width = dec_button.x2 - dec_button.x1 + 1;
			dec_button.height = dec_button.width;
			dec_button.y1 = outline.y1 + 1;
			dec_button.y2 = dec_button.y1 + dec_button.width - 1;

			inc_button.x1 = dec_button.x1;
			inc_button.x2 = dec_button.x2;
			inc_button.width = dec_button.width;
			inc_button.height = dec_button.width;
			inc_button.y2 = outline.y2 - 1;
			inc_button.y1 = inc_button.y2 - inc_button.width + 1;

			trough.x1 = dec_button.x1;
			trough.x2 = dec_button.x2;
			trough.y1 = dec_button.y2 + 1;
			trough.y2 = inc_button.y1 - 1;
			trough.width = (trough.x2 - trough.x1) + 1;
			trough.height = (trough.y2 - trough.y1) + 1;
		}
		break;
	}
	*this->scrollBar      = scroll_bar;
	*this->outline        = outline;
	*this->decreaseButton = dec_button;
	*this->increaseButton = inc_button;
	*this->sliderTrough   = trough;

	this->reconfigure();

	//	When bounds change we must adjust the pixelOffset to reflect
	//	its new relative position.
	
#ifdef DEBUG_OPENOCL_UI_SCROLLBAR
	fprintf( stderr, "ScrollBar::doLayout: offset: min( %i, %i )\n", this->offset, this->maxOffset );
#endif
	this->setOffset( Math::min( this->offset, this->maxOffset ) );

	this->setBoundsChanged( false );
}

void
ScrollBar::setAttributes( unsigned int scrollSize, unsigned int viewableSize )
{
	this->scrollSize = scrollSize;
	this->viewableSize = viewableSize;

	this->reconfigure();

	//this->offset = (int) (this->offset * this->2slider);
}

void
ScrollBar::setIncrement( unsigned int increment )
{
	this->increment = increment;
}

void
ScrollBar::setLogicalOffset( int newOffset )
{
	this->pixelOffset = (int) (newOffset * this->slider2scroll);
	this->reconfigure();
	this->offset = newOffset;

	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}

void
ScrollBar::scrollDecrease()
{
	this->scrollDecrease( this->getIncrement() );
}

void
ScrollBar::scrollIncrease()
{
	this->scrollIncrease( this->getIncrement() );
}

void
ScrollBar::setOffset( int newOffset )
{
	if ( newOffset > this->maxOffset )
	{
		this->offset = this->maxOffset;
	}
	else if ( newOffset >= 0 )
	{
		this->offset = newOffset;
	}
	else
	{
		this->offset = 0;
	}
}

/*
unsigned int
ScrollBar::getPreferredWidth( const GraphicsContext& gc ) const
{
	unsigned int value;
	if ( HORIZONTAL == this->type )
	{
		value = 0xFFFFFFFF;
	} else {
		value = this->bredth;
	}
	return value;
}

unsigned int
ScrollBar::getPreferredHeight( const GraphicsContext& gc ) const
{
	unsigned int value;
	if ( VERTICAL == this->type )
	{
		value = 0xFFFFFFFF;
	} else {
		value = this->bredth;
	}
	return value;
}
*/


Dimensions
ScrollBar::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	switch ( this->type )
	{
	case HORIZONTAL:
		dim.width = 0xFFFF;
		dim.height = this->bredth;
		break;
	case VERTICAL:
		dim.width = this->bredth;
		dim.height = 0xFFFF;
		break;
	}
	return dim;
}

bool
ScrollBar::draw( GraphicsContext& gc ) const
{
	if ( this->hasChanged() )
	{
		Region scroll_bar = *this->scrollBar;
		Region outline    = *this->outline;
		Region dec_button = *this->decreaseButton;
		Region inc_button = *this->increaseButton;
		Region trough     = *this->sliderTrough;
		Region slider     = *this->slider;

		gc.setFill( Color::TOOLBAR );
		gc.drawFilledRectangle( scroll_bar.x1, scroll_bar.y1, scroll_bar.width, scroll_bar.height );
		gc.setFill( Color::SCROLLBAR );
		gc.drawFilledRectangle( trough.x1, trough.y1, trough.width, trough.height );

		this->drawInward3DBox( gc, outline, Color::SHINE2, Color::SHADE2 );

		gc.setFill( Color::TOOLBAR );
		gc.drawFilledRectangle( slider.x1, slider.y1, slider.width, slider.height );

		this->drawOutward3DBox( gc, slider, Color::SHINE2, Color::SHADE2 );

		if ( !this->decPressed )
		{
			this->drawOutward3DBox( gc, dec_button, Color::SHINE2, Color::SHADE2 );
		} else {
			this->drawInward3DBox( gc, dec_button, Color::SHINE1, Color::SHADE1 );
		}
		
		if ( !this->incPressed )
		{
			this->drawOutward3DBox( gc, inc_button, Color::SHINE2, Color::SHADE2 );
		} else {
			this->drawInward3DBox( gc, inc_button, Color::SHINE1, Color::SHADE1 );
		}
	}
	this->setChanged( false );
	return false;
}

/*
 *	Components processEvents to determine whether or not
 *	the event affects it. In the case of a button if the
 *	event occurred within the bounds of the button it
 *	should fire off an Action Event.
 */
void
ScrollBar::processEvent( const Event& anEvent )
{
	bool relayout = false;
	bool redraw   = false;
	bool scrolled = false;

	switch ( anEvent.getType() )
	{
	case Event::MOUSE_EVENT:
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			if ( &me )
			{
				mask bp = MouseEvent::BUTTON1_PRESS;
				mask br = MouseEvent::BUTTON1_RELEASE;
				flags button = me.getButton();
				int me_x = me.getX();
				int me_y = me.getY();

				if ( (bp == ( bp & button )) && this->slider->contains( me_x, me_y ) )
				{
					this->clickX = (me_x - this->getX()) - this->pixelOffset;
					this->clickY = (me_y - this->getY()) - this->pixelOffset;
					//IO::err().printf( "ScrollBar: click %u:%u\n", me.getX(), me.getY() );
					//XXX repaint = true;
				}
				else if ( (bp == ( bp & button )) && this->sliderTrough->contains( me_x, me_y ) )
				{
					this->clickX = 0;
					this->clickY = 0;
					
					unsigned int page_scroll_width  = this->sliderTrough->width;
					unsigned int page_scroll_height = this->sliderTrough->height;
					
					switch ( this->type )
					{
					case HORIZONTAL:
						if ( me_x <= this->slider->x1 )
						{
							this->scrollDecrease( page_scroll_width );
						} else {
							this->scrollIncrease( page_scroll_width );
						}
						break;
					case VERTICAL:
						if ( me_y < this->slider->y1 )
						{
							this->scrollDecrease( page_scroll_height );
						} else {
							this->scrollIncrease( page_scroll_height );
						}
					}
					scrolled = true;
				}
				else if ( (bp == ( bp & button )) && this->decreaseButton->contains( me_x, me_y ) )
				{
					this->decPressed = true;
					redraw = true;
				}
				else if ( (bp == ( bp & button )) && this->increaseButton->contains( me_x, me_y ) )
				{
					this->incPressed = true;
					redraw = true;
				}
				else if ( (br == ( br & button )) && this->decreaseButton->contains( me_x, me_y ) )
				{
					this->clickX = 0;
					this->clickY = 0;
					
					///XXXswitch( this->type )
					//{
					//case HORIZONTAL:
					//	this->fireEvent( ActionEvent( *this, 0, "SCROLL_LEFT.openocl::base::ScrollBar.CLICKED" ) );
					//	break;
					//case VERTICAL:
					//	this->fireEvent( ActionEvent( *this, 0, "SCROLL_UP.openocl::base::ScrollBar.CLICKED" ) );
					//}

					this->decPressed = false;
					this->scrollDecrease( this->increment );
				}
				else if ( (br == ( br & button )) && this->increaseButton->contains( me_x, me_y ) )
				{
					this->clickX = 0;
					this->clickY = 0;
					
					///XXXswitch( this->type )
					//{
					//case HORIZONTAL:
					//	this->fireEvent( ActionEvent( *this, 0, "SCROLL_RIGHT.openocl::base::ScrollBar.CLICKED" ) );
					//	break;
					//case VERTICAL:
					//	this->fireEvent( ActionEvent( *this, 0, "SCROLL_DOWN.openocl::base::ScrollBar.CLICKED" ) );
					//}

					this->incPressed = false;
					this->scrollIncrease( this->increment );
				}
				else // if ( (br == ( br & button )) && (this->clickX || this->clickY) )
				{
					this->decPressed = false;
					this->incPressed = false;
					this->clickX = 0;
					this->clickY = 0;
					//IO::err().printf( "ScrollBar: release %u:%u\n", me.getX(), me.getY() );
					//XXX repaint = true;
				}
			}
		}
		break;
	case Event::MOTION_EVENT:
		{
			const MotionEvent& mo = dynamic_cast<const MotionEvent&>( anEvent );
			if ( &mo && ( this->clickX || this->clickY ) )
			{
	
				//	offset += delta( click + mo )
				//	offset += mo - click
	
				switch ( this->type )
				{
				case HORIZONTAL:
					this->pixelOffset = (mo.getX() - this->getX()) - this->clickX;
					break;
				case VERTICAL:
					this->pixelOffset = (mo.getY() - this->getY()) - this->clickY;

					//IO::err().printf( "ScrollBar::processEvent: pixelOffset = %u - %u - %u\n", mo.getY(), this->getY(), this->clickY );
					break;
				};
		
				this->reconfigure();

				//	When the tab is dragged we we must adjust the offset.
				this->setOffset( (int) (this->pixelOffset * this->scroll2slider) );

				relayout = true;
				scrolled = true;
			}
		}
		break;
	};

	if ( relayout )
	{
		this->fireChangeEvent();
		const_cast<Panel&>( this->getContainingPanel() ).relayout();
	}
	else if ( redraw )
	{
		this->fireChangeEvent();
		const_cast<Panel&>( this->getContainingPanel() ).redraw();
	}

		if ( scrolled )
		{
			//	These aren't needed for the functioning of the scrollbar or the scrollpane.
			//	However, they allow other components, eg. table header, to keep track of
			//	scrollbar movements.
		
			FormattedString offset( "%i", this->offset );
		
			switch ( this->type )
			{
			case HORIZONTAL:
				{
					ActionEvent ae( *this, 0, "HSCROLL.openocl::ui::ScrollBar.MOVED", offset );
					this->fireEvent( ae );
				}
				break;
			case VERTICAL:
				{
					ActionEvent ae( *this, 0, "VSCROLL.openocl::ui::ScrollBar.MOVED", offset );		
					this->fireEvent( ae );
				}
			}
		}

}

unsigned int
ScrollBar::getOffset() const
{
	return this->offset;
}

int
ScrollBar::getIncrement() const
{
	return this->increment;
}

int
ScrollBar::getType() const
{
	return this->type;
}

void
ScrollBar::scrollDecrease( unsigned int increment )
{
		this->setOffset( this->offset - increment );
		this->pixelOffset = (int) (this->offset * this->slider2scroll);
		this->reconfigure();
		
		this->fireChangeEvent();
		this->getContainingPanel().relayout(); // slider region has changed.
}

void
ScrollBar::scrollIncrease( unsigned int increment )
{
		this->setOffset( this->offset + increment );
		this->pixelOffset = (int) (this->offset * this->slider2scroll);
		this->reconfigure();
		
		this->fireChangeEvent();
		this->getContainingPanel().relayout(); // slider region has changed.
}

void
ScrollBar::reconfigure()
{
	Region trough = *this->sliderTrough;
	Region slider;

	int trough_length;
	switch ( type )
	{
	case HORIZONTAL:
		trough_length = trough.width;
		break;
	case VERTICAL:
		trough_length = trough.height;
		break;
	}

	//			  |	|___________|
	//			  |	|			| |
	//	Scroll ---|	|			| |- Viewable Size
	//	Size	  |	|___________| |
	//			  |	|			|
	//

	//	The maximum pixel offset of the component being displayed
	//	is the total scroll size minus the viewable size.
	//	If the viewable size is bigger than scroll size max offset
	//	is set to 0.
	this->maxOffset = this->scrollSize - this->viewableSize;
	if ( this->maxOffset < 0 )
	{
		this->maxOffset = 0;
	}

	//	This ratio allows the slider bar compared to trough represent the
	//	viewable size compared to total size.
	this->view2scroll = (this->viewableSize * 1.0) / (this->scrollSize * 1.0);

	//							Max pixel offset = 
	//								|
	//		--------------------------------------
	//		|		                =============|
	//		--------------------------------------

	this->slider2scroll = (trough_length * 1.0) / (this->scrollSize * 1.0); // 500/1000 = 0.5
	this->scroll2slider = (this->scrollSize * 1.0) / (trough_length * 1.0); // 1000/500 = 2.0

	double tabRatio = this->view2scroll;
	if ( tabRatio > 1.0 ) tabRatio = 1.0;

	int preferred_slider_length = (int) (trough_length * tabRatio);
	int slider_length = Math::max( preferred_slider_length, ScrollBar::minimumSliderLength );
	slider_length = Math::min( slider_length, trough_length );

	this->maxPixelOffset = trough_length - preferred_slider_length;
	if ( this->pixelOffset < 0 )
	{
		this->pixelOffset = 0;
	} else if ( this->pixelOffset > this->maxPixelOffset ) {
		this->pixelOffset = maxPixelOffset;
	}

	//	This adjustment adjusts the slider towards the opposite end as
	//	it approaches an end. This is so that the end will obscure the
	//	button, when the slider is at minimum size.
	int adjustment = 0;
	//if ( slider_length > preferred_slider_length )
	//{
		int max_pixel_offset = Math::max( this->maxPixelOffset, 1 );
		double adjustment_ratio = (this->pixelOffset * 1.0) / (max_pixel_offset * 1.0);
		int difference = slider_length - preferred_slider_length;
		adjustment = (int) (difference * adjustment_ratio);
	//}

	switch ( this->type )
	{
	case HORIZONTAL:
		{
			slider.x1 = trough.x1 + this->pixelOffset - adjustment;
			slider.x2 = slider.x1 + (slider_length - 1);
			slider.y1 = trough.y1;
			slider.y2 = trough.y2;
			slider.width = slider.x2 - slider.x1 + 1;
			slider.height = slider.y2 - slider.y1 + 1;
		}
		break;
	case VERTICAL:
		{
			slider.y1 = trough.y1 + this->pixelOffset - adjustment;
			slider.y2 = slider.y1 + (slider_length - 1);
			slider.x1 = trough.x1;
			slider.x2 = trough.x2;
			slider.width = slider.x2 - slider.x1 + 1;
			slider.height = slider.y2 - slider.y1 + 1;
		}
		break;
	}

	//if ( this->offset > this->maxOffset )
	//{
	//	this->offset = this->maxOffset;
	//}
	//this->pixelOffset = (int) (this->offset * this->offsetToPixelOffsetRatio);

	*this->slider = slider;
}

void
ScrollBar::initArrows( GraphicsContext& gc )
{
	unsigned int w = this->sliderBeginOffset;

	this->lessArrow = gc.createOffscreenImage( w, w );
	this->moreArrow = gc.createOffscreenImage( w, w );
	this->thumbTab  = gc.createOffscreenImage( w, w );
	this->lessArrow->clear();
	this->moreArrow->clear();
	this->thumbTab->clear();

	GraphicsContext& la_gc = this->lessArrow->getGraphicsContext();
	GraphicsContext& ma_gc = this->moreArrow->getGraphicsContext();
	GraphicsContext& tt_gc = this->thumbTab->getGraphicsContext();
	
	la_gc.setForeground( Color::SHADOW_GREY );
	la_gc.drawFilledRectangle( 0, 0, w-1, w-1 );
	la_gc.setForeground( Color::DARK_GREY );
	la_gc.drawFilledRectangle( 0, 0, w-1, w-1 );

	ma_gc.setForeground( Color::SHADOW_GREY );
	ma_gc.drawFilledRectangle( 0, 0, w-1, w-1 );
	ma_gc.setForeground( Color::DARK_GREY );
	ma_gc.drawRectangle( 0, 0, w-1, w-1 );

	switch ( this->type )
	{
	case HORIZONTAL:
		la_gc.setForeground( Color::SHADOW_GREY );
		la_gc.drawLine( 5, 10, 10, 5 );
		la_gc.drawLine( 5, 10, 10, 15 );
		la_gc.setForeground( Color::DARK_GREY );
		la_gc.drawLine( 6, 10, 11, 5 );
		la_gc.drawLine( 6, 10, 11, 15 );
		la_gc.setForeground( Color::SHADOW_GREY );
		la_gc.drawLine( 7, 10, 12, 5 );
		la_gc.drawLine( 7, 10, 12, 15 );

		ma_gc.setForeground( Color::SHADOW_GREY );
		ma_gc.drawLine( 14, 10, 9, 5 );
		ma_gc.drawLine( 14, 10, 9, 15 );
		ma_gc.setForeground( Color::DARK_GREY );
		ma_gc.drawLine( 13, 10, 8, 5 );
		ma_gc.drawLine( 13, 10, 8, 15 );
		ma_gc.setForeground( Color::SHADOW_GREY );
		ma_gc.drawLine( 12, 10, 7, 5 );
		ma_gc.drawLine( 12, 10, 7, 15 );

		break;



	case VERTICAL:
		la_gc.setForeground( Color::SHADOW_GREY );
		la_gc.drawLine( 10, 5, 5, 10 );
		la_gc.drawLine( 10, 5, 15, 10 );
		la_gc.setForeground( Color::DARK_GREY );
		la_gc.drawLine( 10, 6, 5, 11 );
		la_gc.drawLine( 10, 6, 15, 11 );
		la_gc.setForeground( Color::SHADOW_GREY );
		la_gc.drawLine( 10, 7, 5, 12 );
		la_gc.drawLine( 10, 7, 15, 12 );

		ma_gc.setForeground( Color::SHADOW_GREY );
		ma_gc.drawLine( 10, 14, 5, 9 );
		ma_gc.drawLine( 10, 14, 15, 9 );
		ma_gc.setForeground( Color::DARK_GREY );
		ma_gc.drawLine( 10, 13, 5, 8 );
		ma_gc.drawLine( 10, 13, 15, 8 );
		ma_gc.setForeground( Color::SHADOW_GREY );
		ma_gc.drawLine( 10, 12, 5, 7 );
		ma_gc.drawLine( 10, 12, 15, 7 );
		break;
	};

		tt_gc.setForeground( Color::SHADOW_GREY );
		tt_gc.drawLine( 5, 10, 10, 5 );

		tt_gc.setForeground( Color::SHADOW_GREY );
		tt_gc.drawLine( 5, 14, 14, 5 );

		tt_gc.setForeground( Color::SHADOW_GREY );
		tt_gc.drawLine( 9, 14, 14, 9 );

}

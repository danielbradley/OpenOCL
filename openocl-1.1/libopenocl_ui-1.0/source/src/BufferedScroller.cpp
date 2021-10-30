/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/BufferedScroller.h"

#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
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

BufferedScroller::BufferedScroller( Component& aComponent, unsigned int width, unsigned int height )
: Scroller( aComponent )
{
	this->com = &aComponent;
	this->buffer = null;
	this->bufferWidth  = width;
	this->bufferHeight = height;
	this->redrawAll = true;
	
	this->bufferedRegion = new Region();
	this->bufferedRegion->x1 = 0;
	this->bufferedRegion->y1 = 0;
	this->bufferedRegion->width = width;
	this->bufferedRegion->height = height;
	this->bufferedRegion->x2 = width - 1;
	this->bufferedRegion->y2 = height - 1;
}

BufferedScroller::~BufferedScroller()
{
	delete this->buffer;
	delete this->bufferedRegion;
}

void
BufferedScroller::forwardEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	
	if ( Event::CHANGE_EVENT == (Event::CHANGE_EVENT & type ))
	{
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		if ( ChangeEvent::REDRAW_ALL == ce.getValue() )
		{
			//this->Scroller::forwardEvent( anEvent );
			//this->redrawAll = true;
		}
		this->setChanged( true );
	}
	else
	{
		this->Scroller::forwardEvent( anEvent );
	}
}

void
BufferedScroller::doLayout( const GraphicsContext& gc )
{
	bool bounds_changed = this->haveBoundsChanged();

	this->Scroller::doLayout( gc );

	//
	//	XXX - Kludge
	//
	//	It should be possible to dynamically resize buffers on demand, however
	//	if we do this we seem to run out of buffers, therefore I am just creating
	//	once big buffer (hopefully big enough). What is happening to the buffers?
	//	Are they not being freed properly. Have to look into this!

	if ( this->buffer && bounds_changed )
	{
		//fprintf( stderr, "BufferedScroller::doLayout: resizing buffer\n" );
		delete this->buffer;
		this->buffer = null;
	}

	if ( !this->buffer )
	{
		Region bounds = this->getBounds();
		this->bufferWidth = bounds.width;
		this->bufferHeight = bounds.height; //this->bufferHeight;

		this->buffer = gc.createOffscreenImage( this->bufferWidth, this->bufferHeight );
		this->bufferedRegion->width = this->bufferWidth;
		this->bufferedRegion->x2 = this->bufferedRegion->x1 + this->bufferedRegion->width - 1;
		this->bufferedRegion->height = this->bufferHeight;
		this->bufferedRegion->y2 = this->bufferedRegion->y1 + this->bufferedRegion->height - 1;

		this->redrawAll = true;
		this->setChanged( true );
	}
	this->setBoundsChanged( false );
}


//-----------------------------------------------------------------------------
//	public virtual const methods (Component)
//-----------------------------------------------------------------------------

bool
BufferedScroller::draw( GraphicsContext& gc ) const
{
	bool draw_again  = false;
	bool changed     = this->hasChanged();
	bool com_changed = this->com->hasChanged();

	if ( this->buffer )
	{
		GraphicsContext& bgc = this->buffer->getGraphicsContext();
		int x_offset = this->getXOffset();
		int y_offset = this->getYOffset();
		
		Region bounds = this->getBounds();

		//	If the region desired [xOffset:yOffset:bounds.width,bounds.height]
		//	is not within the current bufferedRegion, reposition the buffered region to
		//	include that region, then redraw buffer.
		
		Region target;
		target.x1 = x_offset;
		target.y1 = y_offset;
		target.width = bounds.width;
		target.height = bounds.height;
		target.x2 = target.x1 + target.width - 1;
		target.y2 = target.y1 + target.height - 1;

		if ( changed )
		{
			//	If the scroller has changed it indicates the x and y offsets have been
			//	changed, or a redraw all event has been sent.
			
			//	If a redraw all event has been sent, or if the buffer no long contains
			//	the target region, realign and clear the buffer.
			
			bool buffered = this->bufferedRegion->contains( target );
			if ( !buffered )
			{
				//fprintf( stderr, "BufferedScroller::draw: not buffered\n" );
			}

			if ( this->redrawAll || !buffered )
			{
				int extra_width  = (this->bufferedRegion->width  - bounds.width);
				int extra_height = (this->bufferedRegion->height - bounds.height);
				int buffer_offset_x = extra_width/2;
				int buffer_offset_y = extra_height/2;
			
				this->bufferedRegion->x1 = Math::max( 0, x_offset - buffer_offset_x );
				this->bufferedRegion->x2 = this->bufferedRegion->x1 + this->bufferedRegion->width - 1;
				this->bufferedRegion->y1 = Math::max( 0, y_offset - buffer_offset_y );
				this->bufferedRegion->y2 = this->bufferedRegion->y1 + this->bufferedRegion->height - 1;
			
				bgc.clear();
				//fprintf( stderr, "BufferedScroller::draw: moving buffer region and redrawing\n" );
				const_cast<BufferedScroller*>( this )->com->forwardRedrawAllChangeEvent();
				const_cast<BufferedScroller*>( this )->redrawAll = false;
			}

			//	What is happening with the clip here? Need to fully document this
			//	tomorrow so I can under stand what is going on!

			bgc.setClip( 0, 0, this->bufferWidth, this->bufferHeight );
			bgc.translate( -this->bufferedRegion->x1, -this->bufferedRegion->y1 );
			{
				this->com->draw( bgc );
			}
			bgc.translate( this->bufferedRegion->x1, this->bufferedRegion->y1 );
			bgc.popClip();
		}
		else if ( com_changed )
		{
			//	If only the component has changed, it indicates that an internal
			//	change occured, therefore the just refresh the buffer.

			bgc.setClip( 0, 0, this->bufferWidth, this->bufferHeight );
			bgc.translate( -this->bufferedRegion->x1, -this->bufferedRegion->y1 );
			{
				this->com->draw( bgc );
			}
			bgc.translate( this->bufferedRegion->x1, this->bufferedRegion->y1 );
			bgc.popClip();
		}

		if ( changed || com_changed )
		{
			//	If for any reason anything has changed repaint the buffer to
			//	the gc.

			gc.setClip( bounds.x1, bounds.y1, bounds.width, bounds.height );
			{
				Drawable& src  = *this->buffer;
				//gc.setFill( Color::CANVAS );
				//gc.drawFilledRectangleRegion( bounds );
				gc.copyArea( src, x_offset - this->bufferedRegion->x1, y_offset - this->bufferedRegion->y1, bounds.width, bounds.height, bounds.x1, bounds.y1 );
			}
			gc.popClip();
		
			this->setChanged( false );
		}
	}
	return draw_again;
}

/*
bool
Scroller::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	const int code_path = 1;

	Region bounds = this->getBounds();
	int x          = bounds.x1;
	int y          = bounds.y1;
	unsigned int w = bounds.width;
	unsigned int h = bounds.height;

	gc.setForeground( Color::CANVAS );
	gc.drawFilledRectangle( x, y, w, h );

	if ( this->component && this->buffer )
	{
		if ( this->component->hasChanged() )
		{
			switch ( code_path )
			{
			case 0:
				{
					this->buffer->clear();
					unsigned int buffer_width = this->buffer->getDrawableWidth();
					unsigned int buffer_height = this->buffer->getDrawableHeight();

					GraphicsContext& buffer_gc = this->buffer->getGraphicsContext();
					buffer_gc.setBackground( Color::CANVAS );
					buffer_gc.drawFilledRectangle( 0, 0, buffer_width, buffer_height );
					//  this->buffer->clear();
					draw_again = this->component->draw( buffer_gc );
					//gc.setClip( x, y, w, h );
				}
				break;
			case 1:
			default:
				{
					this->buffer->clear();
					GraphicsContext& buffer_gc = this->buffer->getGraphicsContext();

#ifdef DEBUG_OPENOCL_UI_SCROLLER
					fprintf( stderr, "Scroller::draw: offsets - %i, %i\n", this->xOffset, this->yOffset );
#endif
					buffer_gc.setTranslation( this->xOffset * -1, this->yOffset * -1 );
					draw_again = this->component->draw( buffer_gc );
					buffer_gc.setTranslation( 0, 0, 0 );
				}
			}				
		}
		
		//
		//	Copy the offseted buffer to x, y
		//	

		switch ( code_path )
		{
		case 0:
			gc.copyArea( *this->buffer, this->xOffset, this->yOffset, w, h, x, y );
			break;
		case 1:
		default:
			gc.copyArea( *this->buffer, 0, 0, w, h, x, y );
		}

#ifdef DEBUG_OPENOCL_UI_SCROLLER
		fprintf( stderr, "Scroller::draw: xoffset: %3i, yoffset: %3i\n", this->xOffset, this->yOffset );
#endif
		// IO::err().printf( "Scroller::draw( %u:%u into %u:%u\n", w, h, com_w, com_h );
		//gc.popClip();
	} else if ( !this->component ) {
		//fprintf( stderr, "Scroller::draw(): no component; aborting!\n" );
		abort();
	} else {
		fprintf( stderr, "Scroller::draw(): no buffer; aborting!\n" );
		abort();
	}

	//  gc.setForeground( Color::BLUE );
	//  gc.drawRectangleRegion( this->getBounds() );
	//  gc.drawLine( this->xOffset, y, this->xOffset, bounds.y2 );
	//  gc.drawLine( this->yOffset, x, this->yOffset, bounds.x2 );

	return draw_again;
}
*/

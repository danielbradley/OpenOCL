/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Scroller.h"

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
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

Scroller::Scroller( Component& aComponent )
: Component( "scroller" )
{
	aComponent.addEventListener( *this );
	this->component = &aComponent;

	this->background = new Color( Color::CANVAS );

	this->scrollWidth = 500;
	this->scrollHeight = 500;
	
	this->xOffset = 0;
	this->yOffset = 0;

	this->constrainWidth = false;
	this->constrainHeight = false;
}

Scroller::~Scroller()
{
	this->component->removeEventListener( *this );
	delete this->background;
}

//-------------------------------------------------------------------
//	public virtual methods (EventListener)
//-------------------------------------------------------------------

void
Scroller::deliverEvent( const Event& anEvent )
{
	this->fireEvent( anEvent );
}

//-------------------------------------------------------------------
//	public virtual methods (Component)
//-------------------------------------------------------------------

void
Scroller::setContainingPanel( Panel& aPanel )
{
	Component::setContainingPanel( aPanel );
	this->component->setContainingPanel( aPanel );
}

void
Scroller::doLayout( const GraphicsContext& gc )
{
	Region bounds = this->getBounds();
	int w = bounds.width;
	int h = bounds.height;
	
	Dimensions dim;
	
	if ( this->constrainWidth )
	{
		dim = this->component->getPreferredDimensions( gc, w, 0xFFFF );
	}
	else if ( this->constrainHeight )
	{
		dim = this->component->getPreferredDimensions( gc, 0xFFFF, h );
	} else {
		dim = this->component->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
	}

	this->component->setBounds( 0, 0, dim.width, dim.height );
	this->component->doLayout( gc );
	this->scrollWidth  = dim.width;
	this->scrollHeight = dim.height;
}

void
Scroller::forwardEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	
	if ( Event::POINTER_EVENT == (Event::POINTER_EVENT & type ) )
	{
		Region bounds = this->getBounds();
		const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
		int x = bounds.x1;
		int y = bounds.y1;
				
		int pe_x = pe.getX();
		int pe_y = pe.getY();

		if ( bounds.contains( pe_x, pe_y ) )
		{
			int new_x = (pe_x - x) + xOffset;
			int new_y = (pe_y - y) + yOffset;

			PointerEvent* pe2 = dynamic_cast<PointerEvent*>( pe.copy() );
			pe2->setCoordinates( new_x, new_y );
			this->component->forwardEvent( *pe2 );
			delete pe2;
		}
	} else {
		if ( Event::CHANGE_EVENT == (Event::CHANGE_EVENT & type ))
		{
			this->setChanged( true );
		}

		this->component->forwardEvent( anEvent );
	}
}

//-------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------

void
Scroller::setBackground( const Color& aColor )
{
	Color* color = new Color( aColor );
	delete this->background;
	this->background = color;
}

void
Scroller::setComponent( Component& aComponent )
{
	this->component->removeEventListener( *this );
	aComponent.addEventListener( *this );
	this->component = &aComponent;
	this->xOffset = 0;
	this->yOffset = 0;
}

void
Scroller::setConstrainWidth( bool constrain )
{
	this->constrainWidth = constrain;
}

void
Scroller::setConstrainHeight( bool constrain )
{
	this->constrainHeight = constrain;
}

void
Scroller::setXOffset( unsigned int offset )
{
	if ( offset != this->xOffset )
	{
		this->xOffset = offset;
		this->component->setChanged( true );
	}
}

void
Scroller::setYOffset( unsigned int offset )
{
	if ( offset != this->yOffset )
	{
		this->yOffset = offset;
		this->component->setChanged( true );
	}
}

//-----------------------------------------------------------------------------
//	public virtual const methods (Component)
//-----------------------------------------------------------------------------

/*
unsigned int
Scroller::getPreferredWidth( const GraphicsContext& gc ) const
{
	return this->component->getPreferredWidth( gc );
}

unsigned int
Scroller::getPreferredHeight( const GraphicsContext& gc ) const
{
	return this->component->getPreferredHeight( gc );
}
*/

Dimensions
Scroller::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	if ( this->constrainWidth )
	{
		dim = this->getPreferredDimensions( gc, width, 0xFFFF );
	}
	else if ( this->constrainHeight )
	{
		dim = this->getPreferredDimensions( gc, 0xFFFF, height );
	} else {
		dim = this->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
	}
	return dim;
}

bool
Scroller::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	if ( this->hasChanged() && this->component )// && this->component->hasChanged() )
	{
		Color original_background( gc.getBackground() );
		Region bounds = this->getBounds();
		gc.setClip( bounds.x1, bounds.y1, bounds.width, bounds.height );
		{
			gc.setBackground( *this->background );
			gc.setFill( *this->background );
			gc.drawFilledRectangleRegion( bounds );

			gc.translate( bounds.x1, bounds.y1 );
			{

#ifdef DEBUG_OPENOCL_UI_SCROLLER
				fprintf( stderr, "Scroller::draw: offsets - %i, %i\n", this->xOffset, this->yOffset );
#endif
				gc.translate( this->xOffset * -1, this->yOffset * -1 );
				draw_again = this->component->draw( gc );
				gc.translate( this->xOffset, this->yOffset );
			}
			gc.translate( bounds.x1 * -1, bounds.y1 * -1 );
		}
		gc.popClip();
		
#ifdef DEBUG_OPENOCL_UI_SCROLLER		
		gc.setForeground( Color::RED );
		gc.drawRectangleRegion( bounds );
#endif
		gc.setBackground( original_background );
	}
	this->setChanged( false );
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

const Component&
Scroller::getComponent() const
{
	return *this->component;
}

unsigned int
Scroller::getScrollWidth() const
{
	return this->scrollWidth;
}

unsigned int
Scroller::getScrollHeight() const
{
	return this->scrollHeight;
}

unsigned int
Scroller::getXOffset() const
{
	return this->xOffset;
}

unsigned int
Scroller::getYOffset() const
{
	return this->yOffset;
}

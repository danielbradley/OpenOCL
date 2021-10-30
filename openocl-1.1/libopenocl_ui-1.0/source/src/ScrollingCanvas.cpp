/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Drawable.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/ScrollBar.h"
#include "openocl/ui/ScrollingCanvas.h"
#include "openocl/ui/ScrollPane.h"

#include <openocl/base/Debug.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>

#include <cstdio>

#define SCROLLPANE 1
#define RESET 0

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

//---------------------------------------------------------------------
//	constructors
//---------------------------------------------------------------------

ScrollingCanvas::ScrollingCanvas() : Component( "openocl::ui::ScrollingCanvas" )
{
	this->caller = 0;
	this->scrollPane = new ScrollPane( *this );
	this->setBounds( 0, 0, 800, 1600 );
}

ScrollingCanvas::~ScrollingCanvas()
{
	delete this->scrollPane;
}

//---------------------------------------------------------------------
//	public virtual methods (Container)
//---------------------------------------------------------------------

void
ScrollingCanvas::setContainingPanel( Panel& aPanel )
{
	switch ( this->caller )
	{
	case 1:
		this->Component::setContainingPanel( aPanel );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		this->scrollPane->setContainingPanel( aPanel );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
}

void
ScrollingCanvas::setBounds( int x, int y, unsigned int w, unsigned int h )
{
	switch ( this->caller )
	{
	case 1:
		this->Component::setBounds( x, y, w, h );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		this->scrollPane->setBounds( x, y, w, h );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
}

void
ScrollingCanvas::doLayout( const GraphicsContext& gc )
{
	switch ( this->caller )
	{
	case 1:
		this->doCanvasLayout( gc );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		this->scrollPane->doLayout( gc );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
}

void
ScrollingCanvas::processEvent( const openocl::base::event::Event& anEvent )
{
	switch ( this->caller )
	{
	case 1:
		this->processCanvasEvent( anEvent );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		this->scrollPane->forwardEvent( anEvent );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
}

//---------------------------------------------------------------------
//	public virtual methods (Canvas)
//---------------------------------------------------------------------

void
ScrollingCanvas::doCanvasLayout( const GraphicsContext& gc )
{}

void
ScrollingCanvas::processCanvasEvent( const openocl::base::event::Event& anEvent )
{}

//---------------------------------------------------------------------
//	public virtual constant methods (Container)
//---------------------------------------------------------------------

Dimensions
ScrollingCanvas::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	//	This method should return width:height, when called by its containing context, but
	//	the deterined size of the canvas when called by the scroll pane.

	Dimensions d( width, height );
	/*
	fprintf( stderr, "ScrollingCanvas::getPreferredDimensions: Given: %3i:%3i\n", width, height );

	switch ( this->caller )
	{
	case 1:
		d = this->getPreferredCanvasDimensions( gc, width, height );
		fprintf( stderr, "ScrollingCanvas::getPreferredDimensions: Canvas: %3i:%3i\n", d.width, d.height );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		d = this->scrollPane->getPreferredDimensions( gc, width, height );
		fprintf( stderr, "ScrollingCanvas::getPreferredDimensions: ScrollPane: %3i:%3i\n", d.width, d.height );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
	*/

	return d;
}

bool
ScrollingCanvas::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	Region bounds = this->getBounds();
	
	switch ( this->caller )
	{
	case 1:
		draw_again = this->drawCanvas( gc );
		break;
	case 0:
		const_cast<ScrollingCanvas*>( this )->caller = 1;
		draw_again = this->scrollPane->draw( gc );
		const_cast<ScrollingCanvas*>( this )->caller = 0;
	}
	
	return draw_again;
}

//---------------------------------------------------------------------
//	public virtual constant methods (ScrollingCanvas)
//---------------------------------------------------------------------

/*
Dimensions
ScrollingCanvas::getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	return Dimensions( 1000, 1000 );
}
bool
ScrollingCanvas::drawCanvas( GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	Region box;

	box.x1 = bounds.x1 + 20;
	box.x2 = bounds.x2 - 21;
	box.y1 = bounds.y1 + 20;
	box.y2 = bounds.y2 - 21;

	gc.setForeground( Color( 30, 108, 181 ) );
	gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );

	gc.setForeground( Color::WHITE );
	gc.drawLine( box.x1, box.y1, box.x1, box.y2 );
	gc.drawLine( box.x1, box.y1, box.x2, box.y1 );
	gc.drawLine( box.x1, box.y2, box.x2, box.y2 );
	gc.drawLine( box.x2, box.y1, box.x2, box.y2 );

	//gc.drawRectangle( x + 20, y + 20, w - 40, h - 40 );
	
	return false;
}
*/

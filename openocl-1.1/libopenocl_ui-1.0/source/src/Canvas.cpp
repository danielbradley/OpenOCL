/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Canvas.h"

#include <openocl/base/Debug.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/ui/Drawable.h>
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/ScrollBar.h"

#include <stdio.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

//---------------------------------------------------------------------

//---------------------------------------------------------------------

Canvas::Canvas()
{
	this->scale = 1.0;
	this->setBounds( 0, 0, 800, 1600 );
	this->canvasChanged = true;
}

Canvas::~Canvas()
{}

//---------------------------------------------------------------------
//	public methods (Canvas)
//---------------------------------------------------------------------

void
Canvas::decreaseZoom( double change )
{
	double zoom = this->scale * 100;
	zoom -= change;
	this->setZoom( zoom );
	this->setChanged( true );
}

void
Canvas::increaseZoom( double change )
{
	double zoom = this->scale * 100;
	zoom += change;
	this->setZoom( zoom );
	this->setChanged( true );
}

void
Canvas::setCanvasChanged()
{
	this->canvasChanged = true;
}

void
Canvas::setCanvasDimensions( Dimensions d )
{
	this->dimensions = d;
	this->setChanged( true );
}

void
Canvas::setScale( double scale )
{
	this->scale = scale;
	this->setChanged( true );
}

void
Canvas::setZoom( double zoom )
{
	this->scale = zoom * 0.01; // divide zoom by 100 to get scale
	this->setChanged( true );
}

//---------------------------------------------------------------------
//	public virtual constant methods (Component)
//---------------------------------------------------------------------

bool
Canvas::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	gc.setScale( this->scale );
	draw_again = this->drawCanvas( gc );
	gc.setScale( 1.0 );

	return draw_again;
}

Dimensions
Canvas::getCanvasDimensions() const
{
	return this->dimensions;
}

Dimensions
Canvas::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	double scale = this->scale;
	Dimensions d = this->getPreferredCanvasDimensions( gc, width, height );

	d.width  = (unsigned int) (d.width * scale);
	d.height = (unsigned int) (d.height * scale);

	return d;
}

Dimensions
Canvas::getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	return this->dimensions;
}

//---------------------------------------------------------------------
//	public constant methods (Canvas)
//---------------------------------------------------------------------

bool
Canvas::hasCanvasChanged() const
{
	return this->canvasChanged;
}


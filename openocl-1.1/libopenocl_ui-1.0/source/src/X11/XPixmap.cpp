/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Visual.h"
#include "openocl/ui/X11/XGraphicsContext.h"
#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/X11/XPixmap.h"
#include "openocl/ui/X11/XVisual.h"

#include <openocl/base/NoSuchElementException.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/Palette.h>

#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>

#include <X11/Xlib.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::ui::X11;

XPixmap::XPixmap( const Drawable& aDrawable, unsigned int aWidth,
                                             unsigned int aHeight,
                                             unsigned int aDepth )
: OffscreenImage(), xdrawable( aDrawable )
{
	const XVisual& vis = dynamic_cast<const XVisual&>( aDrawable.getVisual() );
	Display* dpy = (Display*) vis.getXDisplay();

	Window root = DefaultRootWindow( dpy );

	this->xdisplay = dpy;
	this->screen = aDrawable.getScreenId();
	//this->pixmap = XCreatePixmap( dpy, aDrawable.getId(), aWidth, aHeight, aDepth );
	this->pixmap = XCreatePixmap( dpy, root, aWidth, aHeight, aDepth );
	this->width = aWidth;
	this->height = aHeight;
	this->depth = aDepth;
	this->mask = null;
	this->gc = new XGraphicsContext( *this );
	this->gc->resetClip();
}

XPixmap::~XPixmap()
{
	Display* dpy = (Display*) this->xdisplay;

	if ( this->mask )
	{
		XFreePixmap( dpy, (Pixmap) this->mask );
	}
	// XXX-Debug
	delete this->gc;
	this->gc = null;

	XFreePixmap( dpy, (Pixmap) this->pixmap );
}

void
XPixmap::import( const Image& anImage )
{
	//const XVisual& vis = dynamic_cast<const XVisual&>( this->xdrawable->getVisual() );
	Display* dpy = (Display*) this->xdisplay;

	unsigned long valuemask = 0;
	unsigned long panel_id = this->xdrawable.getId();
	XGCValues values;
	this->mask = XCreatePixmap( dpy, panel_id, this->width, this->height, 1 );

	GC mask_gc = XCreateGC( dpy, this->mask, valuemask, &values );
	{
		GraphicsContext& gc = this->getGraphicsContext();
		const Palette& palette = anImage.getPalette();
		unsigned int width = anImage.getWidth();
		unsigned int height = anImage.getHeight();
		const unsigned long** pixels = anImage.getPixelArray();
	
		if ( pixels )
		{
			for ( unsigned int r=0; r < height; r++ )
			{
				for ( unsigned int c=0; c < width; c++ )
				{
					try
					{
						unsigned int color_index = pixels[r][c];
						const Color& color = palette.getColor( color_index );
						gc.setForeground( color );
						gc.drawPoint( c, r );
						XSetForeground( dpy, mask_gc, 0xFFFFFFFF );
						XDrawPoint( dpy, this->mask, mask_gc, c, r );
					} catch ( NoSuchElementException* ex ) {
						delete ex;
						gc.setForeground( Color::WHITE );
						gc.drawPoint( c, r );
						XSetForeground( dpy, mask_gc, 0x00000000 );
						XDrawPoint( dpy, this->mask, mask_gc, c, r );
					}
				}
			}
		}	
	}
	XFreeGC( dpy, mask_gc );
}

void
XPixmap::clear()
{
	unsigned int w = this->getDrawableWidth();
	unsigned int h = this->getDrawableHeight();

	this->gc->resetClip();
	this->gc->setClip( 0, 0, w, h );
	this->gc->setForeground( Color::WHITE );
	this->gc->setBackground( Color::WHITE );
	this->gc->drawFilledRectangle( 0, 0, w, h );
}

void
XPixmap::copyArea(  const Drawable& aDrawable,
                    unsigned int sourceX,
                    unsigned int sourceW,
                    unsigned int sourceWidth,     
                    unsigned int sourceHeight,
                    unsigned int destinationX,
                    unsigned int destinationY ) const
{
	fprintf( stderr, "XPixmap::copyArea( ... ): not implemented, aborting!!\n" );
	fflush( stderr );
	abort();
}

unsigned int
XPixmap::getDrawableWidth() const
{
  return this->width;
}

unsigned int
XPixmap::getDrawableHeight() const
{
  return this->height;
}

unsigned int
XPixmap::getDrawableDepth() const
{
  return this->depth;
}

const openocl::ui::Visual&
XPixmap::getVisual() const
{
  return this->xdrawable.getVisual();
}

const XVisual&
XPixmap::getXVisual() const
{
	return dynamic_cast<const XVisual&>( this->xdrawable.getVisual() );
}

unsigned long
XPixmap::getId() const
{
  return this->pixmap;
}

unsigned long
XPixmap::getScreenId() const
{
	return this->screen;
}

GraphicsContext&
XPixmap::getGraphicsContext() const
{
  return *this->gc;
}

unsigned long
XPixmap::getMask() const
{
	return this->mask;
}

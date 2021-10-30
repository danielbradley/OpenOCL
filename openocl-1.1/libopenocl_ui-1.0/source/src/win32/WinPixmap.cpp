/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/win32/WindowsUndefs.h"

#include "openocl/ui/Drawable.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Visual.h"
#include "openocl/ui/win32/WinGraphicsContext.h"
#include "openocl/ui/win32/WinPixmap.h"
#include "openocl/ui/win32/WinVisual.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/RuntimeException.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/Palette.h>

#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::ui::win32;

int WinPixmap::nrOfPixmaps = 0;
int WinPixmap::sumOfPixmapSizes = 0;

WinPixmap::WinPixmap( const Drawable& aDrawable, unsigned int aWidth,
                                                 unsigned int aHeight,
                                                 unsigned int aDepth )
{
	//	This retrieve a valid DDC for the parent Panel
	//	It is only used to create the bitmap, then released.
	this->bitmapId = null;
	this->maskId   = null;
	this->gc       = null;

	WinGraphicsContext& context = dynamic_cast<WinGraphicsContext&>( aDrawable.getGraphicsContext() );
	HDC ddc = (HDC) context.ddc;
	if ( ddc )
	{
		HBITMAP bitmap_id = CreateCompatibleBitmap( ddc, aWidth, aHeight );
		this->bitmapId = bitmap_id;

		if ( bitmap_id )
		{
			WinPixmap::nrOfPixmaps++;
			WinPixmap::sumOfPixmapSizes += (aWidth * aHeight * 8);
			this->panel = &aDrawable;
			this->gc = new WinGraphicsContext( *this );
			this->width = aWidth;
			this->height = aHeight;
			this->depth = aDepth;
			this->screen = 0;
		} else {
			DWORD error = GetLastError();
			FormattedString message( "WinGraphicsContext::WinGraphicsContext(): cound not create bitmap (%i)", error );
			throw new RuntimeException( message );
		}
	} else {
		fprintf( stderr, "WinPixmap(): abort: only create WinPixmap from WinGraphicsContext::createOffscreenImage(...)\n" );
		abort();
	}

}

WinPixmap::~WinPixmap()
{
	//fprintf( stderr, "WinPixmap::~WinPixmap()\n" );
	HBITMAP bitmap_id = (HBITMAP) this->bitmapId;
	HBITMAP mask_id = (HBITMAP) this->maskId;
	if ( bitmap_id && DeleteObject( bitmap_id ) )
	{
		//fprintf( stderr, "WinPixmap::WinPixmap: deleting nrOfPixmaps: %i\n", WinPixmap::nrOfPixmaps );
		WinPixmap::nrOfPixmaps--;
		WinPixmap::sumOfPixmapSizes -= (this->width * this->height * 8);
	}
	if ( mask_id && DeleteObject( mask_id ) )
	{
	}

	delete this->gc;
}

void
WinPixmap::clear()
{
	this->gc->clear();
}

void
WinPixmap::copyArea(  const Drawable& aDrawable,
                    unsigned int sourceX,
                    unsigned int sourceW,
                    unsigned int sourceWidth,     
                    unsigned int sourceHeight,
                    unsigned int destinationX,
                    unsigned int destinationY ) const
{}

void
WinPixmap::import( const Image& anImage )
{
	WinGraphicsContext& gc = dynamic_cast<WinGraphicsContext&>( this->getGraphicsContext() );
	const Palette& palette = anImage.getPalette();
	unsigned int width = anImage.getWidth();
	unsigned int height = anImage.getHeight();
	const unsigned long** pixels = anImage.getPixelArray();

	HBITMAP mask_id = CreateBitmap( width, height, 1, 1, NULL );
	this->maskId = mask_id;

	HDC mask_dc = CreateCompatibleDC( (HDC) gc.ddc );
	SelectObject( mask_dc, mask_id );
	COLORREF mono = RGB(255,255,255);

	if ( pixels )
	{
		for ( unsigned int r=0; r < height; r++ )
		{
			for ( unsigned int c=0; c < width; c++ )
			{
				unsigned int color_index = pixels[r][c];
				try
				{
					const Color& color = palette.getColor( color_index );
					gc.setForeground( color );
					gc.drawPoint( c, r );

					SetPixel( mask_dc, c, r, mono );

				} catch ( NoSuchElementException* ex ) {
					delete ex;
				}
			}
		}
	}

	DeleteDC( mask_dc );
}

unsigned int
WinPixmap::getDrawableWidth() const
{
  return this->width;
}

unsigned int
WinPixmap::getDrawableHeight() const
{
  return this->height;
}

unsigned int
WinPixmap::getDrawableDepth() const
{
  return this->depth;
}

const openocl::ui::Visual&
WinPixmap::getVisual() const
{
  return this->panel->getVisual();
}

const WinVisual&
WinPixmap::getWinVisual() const
{
	return dynamic_cast<const WinVisual&>( this->panel->getVisual() );
}

unsigned long int
WinPixmap::getId() const
{
  return reinterpret_cast<unsigned long>( this->bitmapId );
}

unsigned long
WinPixmap::getScreenId() const
{
	return this->screen;
}

GraphicsContext&
WinPixmap::getGraphicsContext() const
{
  return *this->gc;
}

//-------------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------------

void*
WinPixmap::getMask()
{
	return this->maskId;
}

void*
WinPixmap::getMask() const
{
	return this->maskId;
}

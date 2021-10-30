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
#include "openocl/ui/Coordinates.h"
#include "openocl/ui/Font.h"
#include "openocl/ui/FontManager.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/UIException.h"
#include "openocl/ui/Visual.h"
#include "openocl/ui/win32/WinGraphicsContext.h"
#include "openocl/ui/win32/WinPixmap.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/Palette.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/Stack.h>

//#include "openocl/ui/X11/XDisplay.h"
//#include "openocl/ui/X11/XFont.h"
//#include "openocl/ui/X11/XPanel.h"
//#include "openocl/ui/X11/XPixmap.h"
//#include "openocl/ui/X11/XVisual.h"
//#include "openocl/ui/Xincludes.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;
using namespace openocl::ui::win32;
using namespace openocl::util;

WinGraphicsContext::WinGraphicsContext( Drawable& aDrawable )
{
	this->pen = null;
	this->brush = null;

	this->resetLineAttributes();

	//	This needs to be last.
	this->paintStruct = NULL;
	this->ddcUsers = 0;
	this->setDrawable( aDrawable ); // also sets this->ddc and this->winId

	//	Except for this
	this->setFont( aDrawable.getVisual().getFontManager().getDefaultFont() );
}

WinGraphicsContext::WinGraphicsContext( const WinGraphicsContext& aDrawable )
{
	abort();
}

WinGraphicsContext::~WinGraphicsContext()
{
	if ( this->ddc ) 
	{
		WinPixmap* win_pixmap = dynamic_cast<WinPixmap*>( this->drawable );
		if ( win_pixmap )
		{
			DeleteDC( (HDC) this->ddc );
		}
		else if ( ((bool) this->paintStruct) )
		{
			EndPaint( (HWND) this->winId, (PAINTSTRUCT*) this->paintStruct );
		}
		else
		{
			ReleaseDC( (HWND) this->winId, (HDC) this->ddc );
		}
	}
}

void
WinGraphicsContext::setDrawable( Drawable& aDrawable )
{
	//Region bounds;
	//bounds.x1 = 0;
	//bounds.y1 = 0;
	//bounds.width = aDrawable.getDrawableWidth();
	//bounds.height = aDrawable.getDrawableHeight();
	//bounds.x2 = bounds.x1 + bounds.width - 1;
	//bounds.y2 = bounds.y1 + bounds.height - 1;

	this->drawable = &aDrawable;
	this->winId = aDrawable.getId();
	this->ddc = null;

	//	If the drawable is a WinPixmap we need to create
	//	a new DDC using the Pixmap's Panel's DDC.

	WinPixmap* win_pixmap = dynamic_cast<WinPixmap*>( &aDrawable );
	if ( win_pixmap )
	{
		HWND panel_id = (HWND) win_pixmap->panel->getId();
		HDC panel_ddc = GetDC( panel_id );
		HDC pixmap_ddc = CreateCompatibleDC( panel_ddc );
		SelectObject( pixmap_ddc, (HGDIOBJ) win_pixmap->bitmapId );
		this->ddc = pixmap_ddc;
		this->ddcUsers++;
		ReleaseDC( panel_id, panel_ddc );
	}
	//*this->clipRegion = bounds;
}

//-----------------------------------------------------------------------------
//	public virtual methods (GraphicsContext): Drawing Methods
//-----------------------------------------------------------------------------

void
WinGraphicsContext::clear()
{
	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		this->setForeground( Color::WHITE );
		this->setFill( Color::WHITE );
		this->setBackground( Color::WHITE );
		RECT rect;
		rect.left = this->getClipOriginX();
		rect.top = this->getClipOriginY();
		rect.right = rect.left + this->getClipWidth();
		rect.bottom = rect.top + this->getClipHeight();
		FillRect( ddc, &rect, (HBRUSH) this->brush );
	}
}

void
WinGraphicsContext::rasterDrawPoint( int x, int y )
{
	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		MoveToEx( ddc, x, y, NULL );
		LineTo( ddc, x+1, y );
	}
}

void
WinGraphicsContext::rasterDrawLine( int x1, int y1, int x2, int y2 )
{
	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		MoveToEx( ddc, x1, y1, NULL );
		LineTo( ddc, x2, y2 );
		MoveToEx( ddc, x2, y2, NULL );
		LineTo( ddc, x1, y1 );
	}
}

/*
void
WinGraphicsContext::drawArrowLine( int X1, int Y1, int X2, int Y2 )
{
	int length = 15;
	int jutt   = 6;

	//
	//		\      |
	//		b/    | dy
	//		  \  |
	//	   ________\|
	//	     dx   A
	//
	//	From the initial line specified by x1:y1 and x2:y2 the angle of the line
	//	can be found using triginometry by finding the tangent (rise/run), which
	//	will they using an arctan lookup yield the angle.
	//
	//	dx = x2 - x1
	//	dy = y2 - y1
	//
	//	Tan A = O/A
	//	Tan A = dy / dx
	//	A     = arctan( dy/dx )

	int dx = X2 - X1;
	int dy = Y2 - Y1;

	int adx = abs( dx );
	int ady = abs( dy );

	double tan_A = (ady * 1.0) / (adx * 1.0);
	double A     = atan( tan_A );

	//	The angle A is represented in radians to convert to degrees. 
	//
	//	Radians are a measure of distance of the unit cirle (circle with circumference 1)
	//	measured in fractiosn of n (pi)
	//
	//	The circumference of a complete circle is 2nr of 2n for the unit circule as r is 1.
	//	Therefore 180 degrees is n, 90 is n/2 ...
	//
	//	 
	//	180 degrees = 1 n radian
	//	1 radian = 180 degrees / n
	//	n readians = n. 180/n
	
	//double Adegrees = A * (180/Math::PI);
	//IO::err().printf( "XGraphicsContext: angle A: %f = atan( %i / %i )\n", Adegrees, dy, dx );

	//
	//	With the angle and an arbitrary distance from the point to (b) the arrowhead
	//	base, we can find the coordinate of this point using cos and sine.
	//	The Sine of an angle equals the length of the opposite side of the length of
	//	the hypotenus. The Cosine of an angle equals the length of the adjacent side
	//	over the length of the hypotenus.
	//
	//	Cos A = A/H			Sin A = O/H
	//	A = H . Cos A			O = H . Sin A

	double adj = length * cos( A );
	double opp = length * sin( A );

	//IO::err().printf( "XGraphicsContext: adj: %f opp: %f\n", adj, opp );

	//	This will give the required distance away from the point x2:y2 for our base line.
	
	int bdx = (int) adj;
	int bdy = (int) opp;

	//	The direction to move the point in each plane is determined by the 

	int sdx = ( dx ? (dx / adx ): 1 );	// either 1 or -1
	int sdy = ( dy ? (dy / ady ): 1 );

	//IO::err().printf( "XGraphicsContext: sdx: %i sdy: %i\n", sdx, sdy );
	
	int bX = X2 - (bdx * sdx);
	int bY = Y2 - (bdy * sdy);

	//IO::err().printf( "XGraphicsContext: bX(%i) = X2(%i) - (bdx(%i) * sdx(%i))\n", bX, X2, bdx, sdx );
	//IO::err().printf( "XGraphicsContext: bY(%i) = Y2(%i) - (bdy(%i) * sdy(%i))\n", bY, Y2, bdy, sdy );


	//		\  d   |
	//	     c  b/    | dy
	//		  \  |
	//	   ________\|
	//	     dx   A
	//
	
	//	We now need to determine flanking points the problem is exaclty the same as the
	//	previous one except that the angles are 90 degrees and 180 degress out. The length
	//	of the hypotinus is now half the width of the arrowhead's base (the jutt).
	//
	//	Sin C = O/H			Cos C = A/H
	//	O = H . Sin C			A = H . Cos C
	
	int    cdx = (int) (jutt * sin( A ));
	int    cdy = (int) (jutt * cos( A ));

	//IO::err().printf( "XGraphicsContext: cdx: %i cdy: %i\n", cdx, cdy );

	int cX = bX + (cdx * sdy);
	int cY = bY + (cdy * sdx * -1);
	
	int dX = bX - (cdx * sdy);
	int dY = bY - (cdy * sdx * -1);
	
	
	//double dy_dx = (dy * 1.0) / (dx * 1.0);
	//double inverse = (dx * 1.0) / (dy * 1.0); 

	//int X3 = X2 - dy;
	//int Y3 = Y2 - dx;

	//int X4 = X1 + (dy - dx);

	//int dx2 = (dy * dy) / dx;
	//int X5 = X1 - dx2;

	//	Now actually do the drawing

	POINT points[3];
	points[0].x = X2;
	points[0].y = Y2;
	points[1].x = cX;
	points[1].y = cY;
	points[2].x = dX;
	points[2].y = dY;

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		this->drawLine( bX, bY, X1, Y1 );
		Polygon( ddc, points, 3 );
	}
}
*/

void
WinGraphicsContext::rasterDrawRectangle( int x, int y, unsigned int width, unsigned int height )
{
	POINT points[5];
	points[0].x = x;
	points[0].y = y;
	points[1].x = x + width - 1;
	points[1].y = y;
	points[2].x = points[1].x;
	points[2].y = y + height - 1;
	points[3].x = x;
	points[3].y = points[2].y;
	points[4] = points[0];

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		Polyline( ddc, points, 5 );
	}

	//	This is stuffing up for some reason
	//
	//Rectangle( (HDC) this->ddc, x, y, (x + width + 1), (y + height + 1) );
}

void
WinGraphicsContext::rasterDrawFilledRectangle( int x, int y, unsigned int width, unsigned int height )
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + width;
	rect.bottom = y + height;

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		FillRect( ddc, &rect, (HBRUSH) this->brush );
	}
}

void
WinGraphicsContext::rasterDrawPolygon( const Sequence& points )
{
	int len = points.getLength();
	POINT* winpoints = new POINT[len];
	
	for ( int i=0; i < len; i++ )
	{
		const Coordinates& c = dynamic_cast<const Coordinates&>( points.elementAt( i ) );
		winpoints[i].x = c.x;
		winpoints[i].y = c.y;
	}
	
	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		Polyline( ddc, winpoints, len );
	}
	delete winpoints;
}

void
WinGraphicsContext::rasterDrawFilledPolygon( const Sequence& points )
{
	int len = points.getLength();
	POINT* winpoints = new POINT[len];
	
	for ( int i=0; i < len; i++ )
	{
		const Coordinates& c = dynamic_cast<const Coordinates&>( points.elementAt( i ) );
		winpoints[i].x = c.x;
		winpoints[i].y = c.y;
	}
	
	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		Polygon( ddc, winpoints, len );
	}
	delete winpoints;
}

void
WinGraphicsContext::rasterDrawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	Region box;
	box.x1 = x;
	box.y1 = y;
	box.width = width;
	box.height = height;
	box.x2 = box.x1 + box.width - 1;
	box.y2 = box.y1 + box.height - 1;

	double degrees_to_radians = Math::pi()/180;

	double h = width/2.0;
	double a1_degrees = angle1 / 64.0;
	double a2_degrees = a1_degrees + (angle2 / 64.0);
	double a1_radians = a1_degrees * degrees_to_radians;
	double a2_radians = a2_degrees * degrees_to_radians;

	//	sin t = o/h			cos t = a/h
	//	o = h.sin t			a = h.cos t
	//	o ~ y				a ~ x
	double cos_a1 = cos( a1_radians );
//	fprintf( stderr, "WGC:   cos_a1: %f\n", cos_a1 );
	double h_cos_a1 = h * cos_a1;
//	fprintf( stderr, "WGC: h_cos_a1: %f\n", h_cos_a1 );

	int a1dx = (int) (h * cos( a1_radians ));
	int a1dy = (int) (h * sin( a1_radians ));

	int a2dx = (int) (h * cos( a2_radians ));
	int a2dy = (int) (h * sin( a2_radians ));

	int a1x = box.x1 + ((int) h) + a1dx;
	int a1y = box.y1 + ((int) h) - a1dy;

	int a2x = box.x1 + ((int) h) + a2dx;
	int a2y = box.y1 + ((int) h) - a2dy;


	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		if ( angle2 > 0 )
		{
			SetArcDirection( ddc, AD_COUNTERCLOCKWISE );
		} else {
			SetArcDirection( ddc, AD_CLOCKWISE );
		}
		Arc( ddc, box.x1, box.y1, box.x2, box.y2, a1x, a1y, a2x, a2y );
	}
}

void
WinGraphicsContext::rasterDrawArcFromCoords( Region box, int x1, int y1, int x2, int y2 )
{
	//	box is the region that indicates the bounds of the undrawn oval
	//	x1:y1 is the start of the arc
	//	x1:y1 is the end of the arc

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		Arc( ddc, box.x1, box.y1, box.x2, box.y2, x1, y1, x2, y2 );
	}
}

void
WinGraphicsContext::rasterDrawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	int x2 = x + width - 1;
	int y2 = y + height - 1;

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		Pie( ddc, x, y, x2, y2, x, y, x, y );
	}
}

void
WinGraphicsContext::rasterDrawString( const String& aString, int x, int y )
{
	TEXTMETRIC text_metrics;
	RECT rect;

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		if ( this->activeFont )
		{
			SelectObject( ddc, (HFONT) this->activeFont->getFontId() );
		} else {
			fprintf( stderr, "Aborted in WinGraphicsContext::drawString: no active font\n" );
			abort();
		}
		if ( GetTextMetrics( ddc, &text_metrics ) )
		{
			TextExtents* extents = this->measureTextExtents( aString );

			rect.left = x;
			rect.top  = y;
			rect.top -= text_metrics.tmAscent;
			rect.right = rect.left + extents->width;
			rect.bottom = rect.top + extents->ascent + extents->descent;

			switch ( 1 )
			{
			case 0:
				TextOut( (HDC) this->ddc, x, y, aString.getChars(), aString.getLength() );
				break;
			case 1:
				ExtTextOut( (HDC) this->ddc, rect.left, rect.top, ETO_OPAQUE, &rect, aString.getChars(), aString.getLength(), 0 );
				break;
			}
			delete extents;
		}
	}
}

/*
void
WinGraphicsContext::drawBoundedString( const String& aString, const Region& aRegion, flags options )
{
	TEXTMETRIC text_metrics;
	HDC ddc = (HDC) this->ddc;

	RECT rect;
	rect.left   = aRegion.x1;
	rect.top    = aRegion.y1;
	rect.right  = aRegion.x2;
	rect.bottom = aRegion.y2;

	HDC ddc = (HDC) this->ddc;
	switch ( (long int) ddc )
	{
	case 0:
		fprintf( stderr, "WinGraphicsContext warning: attempting to draw without valid context!\n" );
		break;
	default:
		if ( GetTextMetrics( ddc, &text_metrics ) )
		{
			rect.bottom += text_metrics.tmDescent;
			DrawText( ddc, aString.getChars(), aString.getLength(), &rect, options );
		}
	}
}
*/

void
WinGraphicsContext::rasterDrawImage( const openocl::ui::OffscreenImage& anImage, int x, int y )
{
	unsigned int w = anImage.getDrawableWidth();
	unsigned int h = anImage.getDrawableHeight();

	this->copyArea( anImage, 0, 0, w, h, x, y );
}

void
WinGraphicsContext::rasterDrawImage( const openocl::imaging::Image& anImage, int x, int y )
{
	unsigned int cols = anImage.getWidth();
	unsigned int rows = anImage.getHeight();

	const Palette& palette = anImage.getPalette();
	const unsigned long** pixels = anImage.getPixelArray();

	for ( unsigned int r=0; r < rows; r++ )
	{
		for ( unsigned int c=0; c < cols; c++ )
		{
			//	Default mode for xserver seems to be true color
			//	rather than using color lookup. Need to
			//	introduce test here the determines
			//	which system is being used and handle appropriately.

			try {
				unsigned int index = pixels[r][c];
				const Color& color = palette.getColor( index );
				this->setForeground( color );
				this->drawPoint( x, y );
			} catch ( NoSuchElementException* ex ) {
				delete ex;
			}
		}
	}
}


//-----------------------------------------------------------------------------
//	public virtual methods (GraphicsContext): context methods
//-----------------------------------------------------------------------------

void
WinGraphicsContext::flush()
{
	GdiFlush();
}

void
WinGraphicsContext::setForeground( const Color& aColor )
{
	this->GraphicsBase::setBrush( aColor );

	long col = aColor.getColor();
	this->foreground = col;
	this->fill       = col;
	this->background = col;
	this->winRefreshPen();
	this->winRefreshBrush();
}

void
WinGraphicsContext::setFill( const Color& aColor )
{
	this->GraphicsBase::setFill( aColor );

	this->fill = aColor.getColor();
	this->winRefreshBrush();
}

void
WinGraphicsContext::setBackground( const Color& aColor )
{
	this->GraphicsBase::setBackground( aColor );

	this->background = aColor.getColor();
	SetBkColor( (HDC) this->ddc, this->background );
}

void
WinGraphicsContext::setFillStyle( mask style )
{
}

void
WinGraphicsContext::setFillRule( mask rule )
{
}

void
WinGraphicsContext::setDefaultFont() const
{
	this->setFont( this->drawable->getVisual().getFontManager().getDefaultFont() );
}

void
WinGraphicsContext::setFont( const openocl::ui::Font& aFont ) const
{
	const_cast<WinGraphicsContext*>( this )->activeFont = &aFont;
	//fprintf( stderr, "WinGraphicsContext::setFont: this = %x ddc = %x\n", this, ddc );
	//SelectObject( (HDC) this->ddc, (HFONT) aFont.getFontId() );
}

//void
//WinGraphicsContext::setFont( XFont* font )
//{
//}

openocl::ui::OffscreenImage*
WinGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight ) const
{
	OffscreenImage* pixmap = null;

	if ( this->ddc )
	{
        pixmap = this->createOffscreenImage( aWidth, aHeight, 0 );
	} else {
		this->beginPaint( NULL );
        pixmap = this->createOffscreenImage( aWidth, aHeight, 0 );
		this->endPaint( NULL );
	}
	if ( pixmap && (0 == pixmap->getId()) )
	{
		delete pixmap;
		pixmap = null;
	}
	return pixmap;
}

openocl::ui::OffscreenImage*
WinGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const
{
	WinPixmap* pixmap = null;
	
	if ( this->ddc )
	{
		pixmap = new WinPixmap( *this->drawable, aWidth, aHeight, aDepth );
	} else {
		this->beginPaint( NULL );
		pixmap = new WinPixmap( *this->drawable, aWidth, aHeight, aDepth );
		this->endPaint( NULL );
	}
	if ( 0 == pixmap->getId() )
	{
		delete pixmap;
		pixmap = null;
	}
	return pixmap;
}

openocl::ui::OffscreenImage*
WinGraphicsContext::createOffscreenImage( const Image& anImage ) const
{
	WinPixmap* pixmap = null;
	
	if ( this->ddc )
	{
		pixmap = new WinPixmap( *this->drawable, anImage.getWidth(), anImage.getHeight(), this->drawable->getDrawableDepth() );
	} else {
		this->beginPaint( NULL );
		pixmap = new WinPixmap( *this->drawable, anImage.getWidth(), anImage.getHeight(), this->drawable->getDrawableDepth() );
		this->endPaint( NULL );
	}

	GraphicsContext& gc = pixmap->getGraphicsContext();
	switch ( (unsigned long) pixmap->bitmapId )
	{
	case NULL:
		delete pixmap;
		pixmap = null;
		break;
	default:
        pixmap->import( anImage );
	}
	return pixmap;
}

void
WinGraphicsContext::resetLineAttributes()
{
	this->lineAttributes.width = 1;
	this->lineAttributes.lineStyle = PS_SOLID;
	this->lineAttributes.capStyle = 0;
	this->lineAttributes.joinStyle = 0;

	this->winRefreshPen();
}

void
WinGraphicsContext::setLineAttributes( unsigned int width,
                                     int          lineStyle,
                                     int          capStyle,
                                     int          joinStyle )
{
	this->lineAttributes.width     = width;
	this->lineAttributes.lineStyle = lineStyle;
	this->lineAttributes.capStyle  = capStyle;
	this->lineAttributes.joinStyle = joinStyle;

	this->winRefreshPen();
}

void
WinGraphicsContext::setLineDashes( unsigned int dashOffset,
                                 char dashList[],
                                 unsigned int n )
{
}




unsigned int
WinGraphicsContext::measureTextWidth( const String& aString ) const
{
	unsigned int value = 0;
	this->beginPaint( NULL );
	{
		HDC ddc = (HDC) this->ddc;
		SIZE size;

		SelectObject( ddc, (HFONT) this->activeFont->getFontId() );

		if ( GetTextExtentPoint32( ddc, aString.getChars(), aString.getLength(), &size ) )
		{
			value = size.cx;
		} else {
			fprintf( stderr, "WinGraphicsContext::measureTextHeight(): failed\n" );
		}
	}
	this->endPaint( NULL );
	return value;
}

unsigned int
WinGraphicsContext::measureTextHeight( const String& aString ) const
{
	unsigned int value = 0;
	this->beginPaint( null );
	{
		HDC ddc = (HDC) this->ddc;
		SIZE size;

		SelectObject( ddc, (HFONT) this->activeFont->getFontId() );

		if ( GetTextExtentPoint32( ddc, aString.getChars(), aString.getLength(), &size ) )
		{
			value = size.cy;
		} else {
			fprintf( stderr, "WinGraphicsContext::measureTextHeight(): failed\n" );
		}
	}
	this->endPaint( NULL );
	return value;
}

TextExtents*
WinGraphicsContext::measureTextExtents( const String& aString ) const
{
	TextExtents* ext = new TextExtents();

	this->beginPaint( null );
	{
		HDC ddc = (HDC) this->ddc;

		SelectObject( ddc, (HFONT) this->activeFont->getFontId() );

		SIZE size;
		TEXTMETRIC text_metrics;

		GetTextExtentPoint32( ddc, aString.getChars(), aString.getLength(), &size );
		GetTextMetrics( ddc, &text_metrics );
		ext->width = size.cx;
		ext->ascent = text_metrics.tmAscent;
		ext->descent = text_metrics.tmDescent;
		ext->direction = 0;
	}
	this->endPaint( null );
	return ext;
}

void
WinGraphicsContext::copyArea( const Drawable& source,
                            unsigned int sourceX,
                            unsigned int sourceY,
                            unsigned int sourceWidth,
                            unsigned int sourceHeight,
                            Drawable& destination,
                            unsigned int destinationX,
                            unsigned int destinationY )
{ 
}

void
WinGraphicsContext::copyArea( const Drawable& source,
                            unsigned int srcX,
                            unsigned int srcY,
                            unsigned int srcWidth,
                            unsigned int srcHeight,
                            unsigned int destX,
                            unsigned int destY )
{
	const WinPixmap& pixmap = dynamic_cast<const WinPixmap&>( source );

	this->beginPaint( null );
	HBITMAP hbmMask = (HBITMAP) pixmap.getMask();
	HDC ddc = (HDC) this->ddc;
	HDC source_ddc = (HDC) dynamic_cast<WinGraphicsContext&>( source.getGraphicsContext() ).ddc;
	HWND panel_id = (HWND) this->drawable->getId();

	DWORD raster = MAKEROP4(SRCCOPY,SRCPAINT);

	switch ( (int) ddc )
	{
	case 0:
		break;
	default:
		switch ( (int) source_ddc )
		{
		case 0:
			break;
		default:
			if ( hbmMask )
			{
				if ( !MaskBlt( ddc, destX, destY, srcWidth, srcHeight, source_ddc, srcX, srcY, hbmMask, 0, 0, raster ) )
				{
					fprintf( stderr, "Maskblt failed\n" );
				}
			}
			else
			{
				if ( !BitBlt( ddc, destX, destY, srcWidth, srcHeight, source_ddc, srcX, srcY, SRCCOPY ) )
				{
					fprintf( stderr, "Bitblt failed\n" );
				}
			}
		}
	}
	this->endPaint( null );
}

void
WinGraphicsContext::copyAreaFrom(
                            unsigned int sourceX,
                            unsigned int sourceY,
                            unsigned int sourceWidth,
                            unsigned int sourceHeight,
                            Drawable& destination,
                            unsigned int destinationX,
                            unsigned int destinationY )
{ 
}


void
WinGraphicsContext::copyAreaFromParent( const Drawable& source,
                                      unsigned int sourceX,
                                      unsigned int sourceY,
                                      unsigned int sourceWidth,
                                      unsigned int sourceHeight,
                                      unsigned int destinationX,
                                      unsigned int destinationY )
{
}

void
WinGraphicsContext::clip( ClipArea clip )
{
	HDC ddc = (HDC) this->ddc;

	switch ( (int) ddc )
	{
	case 0:
		break;
	default:
        Region clip_region;
		clip_region.x1     = clip.x;
		clip_region.y1     = clip.y;
		clip_region.width  = clip.width;
		clip_region.height = clip.height;
		clip_region.x2     = clip_region.x1 + clip_region.width - 1;
		clip_region.y2     = clip_region.y1 + clip_region.height - 1;

		IntersectClipRect( ddc, clip_region.x1, clip_region.y1, clip_region.x2 + 1, clip_region.y2 + 1 );
	}
}

void
WinGraphicsContext::unclip( ClipArea clip )
{
	HDC ddc = (HDC) this->ddc;

	switch ( (int) ddc )
	{
	case 0:
		break;
	default:
		HRGN rgn = CreateRectRgn( clip.x, clip.y, clip.x + clip.width, clip.y + clip.height );
		ExtSelectClipRgn( ddc, rgn, RGN_OR );
		DeleteObject( rgn );
	}
}

//unsigned int
//WinGraphicsContext::getWidth()
//{
//  return this->width;
//}

//unsigned int
//WinGraphicsContext::getHeight()
//{
//  return this->height;
//}

//unsigned int
//WinGraphicsContext::getBorderWidth()
//{
//  return this->borderWidth;
//}

unsigned int
WinGraphicsContext::getClipOriginX() const
{
	return this->getClipRegion().x1;
}

unsigned int
WinGraphicsContext::getClipOriginY() const
{
	return this->getClipRegion().y1;
}

unsigned int
WinGraphicsContext::getClipWidth() const
{
	return this->getClipRegion().width;
}

unsigned int
WinGraphicsContext::getClipHeight() const
{
	return this->getClipRegion().height;
}

//const XFont&
//WinGraphicsContext::getActiveFont() const
//{
//  return *this->activeFont;
//}

void
WinGraphicsContext::initializeDefaultFont()
{
}

void
WinGraphicsContext::winRefreshPen()
{
	HPEN hpen;
	if ( this->pen )
	{
		hpen = (HPEN) this->pen;
		DeleteObject( hpen );
	}

	DWORD style = 0;
	if ( GraphicsContext::SOLID_LINE == (GraphicsContext::SOLID_LINE & this->lineAttributes.lineStyle) )
	{
		style |= PS_SOLID;
	}
	if ( GraphicsContext::DASHED_LINE == (GraphicsContext::DASHED_LINE & this->lineAttributes.lineStyle) )
	{
		style |= PS_DASH;
	}
	//if ( ROUND_ENDCAP == (ROUND_ENDCAP & this->lineAttributes.capStyle) )
	//{
	//	
	//}

	//hpen = CreatePen( PS_SOLID, 1, this->foreground );
	hpen = CreatePen( style, this->lineAttributes.width, this->foreground );
	this->pen   = (void*) hpen;
	SelectObject( (HDC) this->ddc, hpen );
	SetTextColor( (HDC) this->ddc, this->foreground );
}

void
WinGraphicsContext::winRefreshBrush()
{
	HBRUSH hbrush;
	if ( this->brush )
	{
		hbrush = (HBRUSH) this->brush;
		DeleteObject( hbrush );
	}

	hbrush = CreateSolidBrush( this->fill );
	this->brush = (void*) hbrush;
	SelectObject( (HDC) this->ddc, hbrush );
}

void
WinGraphicsContext::beginPaint( void* paintStruct ) const
{
	this->setFont( this->drawable->getVisual().getFontManager().getDefaultFont() );

	if ( 0 == this->ddcUsers )
	{
		switch ( (long int) paintStruct )
		{
		case NULL:
			//fprintf( stderr, "WingraphicsContext::beginPaint: NULL\n" );
			const_cast<WinGraphicsContext*>( this )->ddc = GetDC( (HWND) this->winId );
			break;
		default:
			const_cast<WinGraphicsContext*>( this )->ddc = BeginPaint( (HWND) this->winId, (PAINTSTRUCT*) paintStruct );
			const_cast<WinGraphicsContext*>( this )->paintStruct = paintStruct;
			//fprintf( stderr, "WingraphicsContext::beginPaint: %x --> %x\n", this->winId, this->ddc );
		}
		if ( !this->ddc )
		{
			String message( "WinGraphicsContext::beginPaint: could not acquire DDC\n" );
			throw new RuntimeException( message );
		}
	}
	const_cast<WinGraphicsContext*>( this )->ddcUsers++;

	//	Select active Font into ddc. Actually this should be handled by methods using fonts.
	//
	//	if ( this->activeFont )
	//	{
	//		SelectObject( (HDC) this->ddc, (HFONT) this->activeFont->getFontId() );
	//	} else {
	//		printf( stderr, "Aborted in WinGraphicsContext::beginPaint: no active font\n" );
	//		abort();
	//	}
}

void
WinGraphicsContext::endPaint( void* paintStruct ) const
{
	if ( 1 == this->ddcUsers )
	{
		switch ( (long int) paintStruct )
		{
		case NULL:
			ReleaseDC( (HWND) this->winId, (HDC) this->ddc );
			break;
		default:
			EndPaint( (HWND) this->winId, (PAINTSTRUCT*) paintStruct );
			const_cast<WinGraphicsContext*>( this )->paintStruct = null;
		}
		const_cast<WinGraphicsContext*>( this )->ddc = null;
	}
	const_cast<WinGraphicsContext*>( this )->ddcUsers--;
}

FontManager&
WinGraphicsContext::getFontManager() const
{
	return this->drawable->getVisual().getFontManager();
}

Visual&
WinGraphicsContext::getVisual() const
{
	return const_cast<Visual&>( this->drawable->getVisual() );
}

Drawable&
WinGraphicsContext::getDrawable() const
{
	return *this->drawable;
}

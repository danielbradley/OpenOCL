/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/X11/XGraphicsContext.h"

#include "openocl/ui/Font.h"
#include "openocl/ui/FontManager.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/UIException.h"
#include "openocl/ui/X11/XColorRGB.h"
#include "openocl/ui/X11/XFont.h"
#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/X11/XPixmap.h"
#include "openocl/ui/X11/XVisual.h"
#include "openocl/ui/Xincludes.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/Palette.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/ADTObject.h>
#include <openocl/util/Stack.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>
#include <cmath>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;
using namespace openocl::ui::X11;

XGraphicsContext::XGraphicsContext( Drawable& aDrawable )
{
	Debug::entering( "XGraphicsContext", "XGraphicsContext", "XDrawable&" );

	this->setDrawable( aDrawable );
	//  this->panel = &aPanel;
	//this->display = (Display*) aPanel.xvisual->getXDisplay().getDisplay();
	//this->win = aPanel.winId;

	Display* d = (Display*) this->display;

	unsigned long valuemask = 0;
	//valuemask |= GCLineWidth;
	//valuemask |= GCLineStyle;

	XGCValues values;
	//values.line_width = 1;
	//values.line_style = LineSolid;
	

	this->gc = XCreateGC( d, win, valuemask, &values );
	if ( this->gc )
	{
		XSetClipOrigin( d, (GC) this->gc, 0, 0 );
		XSetClipMask( d, (GC) this->gc, None );

//		XRectangle r;
//		r.x = 0;
//		r.y = 0;
//		r.width  = 0xFFFF;
//		r.height = 0xFFFF;
//		XSetClipRectangles( d, (GC) this->gc, 0, 0, &r, 1, Unsorted ); 
	}
	//fprintf( stderr, "XGraphicsContext::creating gc: 0x%x --> 0x%x\n", d, this->gc );

  if ( null == this->gc )
  {
	abort();
    IO::out().printf( "could not get gc\n" );
  }
  this->initializeDefaultFont();
  
  this->setForeground( Color::BLACK );
  this->setFill( Color::CANVAS );
  this->setBackground( Color::CANVAS );


  Debug::leaving();
}

XGraphicsContext::~XGraphicsContext()
{
	Display* dpy = (Display*) this->display;
	if ( this->gc )
	{
		//fprintf( stderr, "XGraphicsContext::freeing gc: 0x%x --> 0x%x\n", dpy, this->gc );

#ifndef OPENOCL_OSX_KLUDGE
		XFreeGC( dpy, (GC) this->gc );
#endif
		this->gc = null;
	} else {
		fprintf( stderr, "XGraphicsContext::~XGraphicsContext: no gc!!\n" );
	}
}

void
XGraphicsContext::initializeDefaultFont()
{
	/*
	XFont* defaultFont = null;
	const XVisual& xvisual = dynamic_cast<const XVisual&>( this->xdrawable->getVisual() );
	if ( &xvisual )
	{
		defaultFont = xvisual.getXDisplay().loadFont( "fixed" );
		this->setFont( defaultFont );
	}
	*/
	
	const XVisual& xvisual = dynamic_cast<const XVisual&>( this->xdrawable->getVisual() );
	const Font& font = xvisual.getFontManager().getDefaultFont();
	this->setFont( font );
}

void
XGraphicsContext::setDrawable( Drawable& aDrawable )
{
	const XVisual& xvisual = dynamic_cast<const XVisual&>( aDrawable.getVisual() );
	if ( &xvisual )
	{
		this->xdrawable = &aDrawable;
		this->display = (Display*) xvisual.getXDisplay();
		this->win = this->xdrawable->getId();
	} else {
		throw new UIException( "invalid drawable for graphics context" );
	}
}

void
XGraphicsContext::clear()
{
	this->setForeground( Color::WHITE );
	this->setBackground( Color::WHITE );
	
	//	This is kinda reckless, but clip should be set anyway.
	this->drawFilledRectangle( 0, 0, 0xFFFF, 0xFFFF );
}

void
XGraphicsContext::flush()
{}


void
XGraphicsContext::setForeground( const Color& aColor )
{
	this->GraphicsBase::setBrush( aColor );

	XColorRGB xcolor( aColor );

	//	Default mode for xserver seems to be true color
	//	rather than using color lookup. Need to
	//	introduce test here the determines
	//	which system is being used and handle appropriately.

	Display* dpy = (Display*) this->display;
	
	
	switch ( TRUE_COLOR )
	{
	case TRUE_COLOR:
		XSetForeground( dpy, (GC) this->gc, xcolor.getColor() );
		break;
	case INDEXED:
		//{
		//	unsigned int screen = this->xdrawable->getScreen();
		//	Colormap colormap = DefaultColormap( dpy, screen );
		//	XColor xcolor;
		//	xcolor.red   = aColor.getRed();
		//	xcolor.green = aColor.getGreen();
		//	xcolor.blue  = aColor.getBlue();
		//	
		//	if ( 0 == XAllocColor( dpy, colormap, &xcolor ) )
		//	{
		//		IO::err().printf( "Could not alloate color!\n" );
		//	}
		//}
		//	XSetForeground( (Display*) this->display, (GC) this->gc, xcolor.pixel );
		break;
	}
}

void
XGraphicsContext::setFill( const Color& aColor )
{
	this->GraphicsBase::setFill( aColor );

	XColorRGB xcolor( aColor );
	Display* dpy = (Display*) this->display;
	XSetForeground( dpy, (GC) this->gc, xcolor.getColor() );
}

void
XGraphicsContext::setBackground( const Color& aColor )
{
	this->GraphicsBase::setBackground( aColor );

	XColorRGB xcolor( aColor );
	Display* dpy = (Display*) this->display;
	XSetBackground( dpy, (GC) this->gc, xcolor.getColor() );
}

/*
void
XGraphicsContext::setBackground( const Color& aColor )
{
  XSetBackground( (Display*) this->display, (GC) this->gc, aColor.getColor() );
}  
*/
void
XGraphicsContext::setFillStyle( mask style )
{
  XSetFillStyle( (Display*) this->display, (GC) this->gc, style );
}

void
XGraphicsContext::setFillRule( mask rule )
{
  XSetFillRule( (Display*) this->display, (GC) this->gc, rule );
}

void
XGraphicsContext::setDefaultFont() const
{
	this->setFont( this->getFontManager().getDefaultFont() );
}

void
XGraphicsContext::setFont( const Font& font ) const
{
	this->GraphicsBase::setFont( font );

	void* adt = font.getFontId();
	if ( adt )
	{
		XFontStruct* xfont = reinterpret_cast<XFontStruct*>( adt );
		XSetFont( (Display*) this->display, (GC) this->gc, xfont->fid );
	}
}

openocl::ui::OffscreenImage*
XGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight )
const
{
	unsigned int depth = this->xdrawable->getDrawableDepth();
	return new XPixmap( *this->xdrawable, aWidth, aHeight, depth );
}

openocl::ui::OffscreenImage*
XGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const
{
	return new XPixmap( *this->xdrawable, aWidth, aHeight, aDepth );
}

openocl::ui::OffscreenImage*
XGraphicsContext::createOffscreenImage( const Image& anImage ) const
{
	XPixmap* pixmap = new XPixmap( *this->xdrawable, anImage.getWidth(), anImage.getHeight(), this->xdrawable->getDrawableDepth() );
	pixmap->import( anImage );
	return pixmap;
}

void
XGraphicsContext::resetLineAttributes()
{
	this->setLineAttributes( 1, GraphicsContext::SOLID_LINE, 0, 0 );
}

void
XGraphicsContext::setLineAttributes( unsigned int aWidth,
                                     int          aLineStyle,
                                     int          aCapStyle,
                                     int          aJoinStyle )
{
	int lineStyle = LineSolid;
	int capStyle  = CapRound;
	int joinStyle = JoinRound;

	switch ( aLineStyle )
	{
	case GraphicsContext::DASHED_LINE:
		lineStyle = LineDoubleDash;
		break;
	case GraphicsContext::SOLID_LINE:
	default:
		lineStyle = LineSolid;
	}
		
	XSetLineAttributes( (Display*) this->display, (GC) this->gc,
			aWidth, lineStyle, capStyle, joinStyle );
}

void
XGraphicsContext::setLineDashes( unsigned int dashOffset,
                                 char dashList[],
                                 unsigned int n )
{
  XSetDashes( (Display*) this->display, (GC) this->gc, dashOffset, dashList, n );
}

//void
//XGraphicsContext::setFont( const Font& font ) const
//{
//
//}

void
XGraphicsContext::rasterDrawPoint( int x, int y )
{
	XDrawPoint( (Display*) this->display, this->win, (GC) this->gc, x, y );
}

void
XGraphicsContext::rasterDrawLine( int x1, int y1, int x2, int y2 )
{
	XDrawLine( (Display*) this->display, this->win, (GC) this->gc, x1, y1, x2, y2 );
}

/*
void
XGraphicsContext::drawArrowLine( int X1, int Y1, int X2, int Y2 )
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
	
	
	//
	//double dy_dx = (dy * 1.0) / (dx * 1.0);
	//double inverse = (dx * 1.0) / (dy * 1.0); 
	//
	//int X3 = X2 - dy;
	//int Y3 = Y2 - dx;
	//
	//int X4 = X1 + (dy - dx);
	//
	//int dx2 = (dy * dy) / dx;
	//int X5 = X1 - dx2;
	//

	//	Now actually do the drawing

	XPoint points[3];
	points[0].x = X2;
	points[0].y = Y2;
	points[1].x = cX;
	points[1].y = cY;
	points[2].x = dX;
	points[2].y = dY;
	
	XDrawLine( (Display*) this->display, this->win, (GC) this->gc, X1, Y1, bX, bY );
	XFillPolygon( (Display*) this->display, this->win, (GC) this->gc, points, 3, 0, CoordModeOrigin );
}
*/

void
XGraphicsContext::rasterDrawRectangle( int x, int y, unsigned int width, unsigned int height )
{
	Display* dpy = (Display*) this->display;
	XDrawRectangle( dpy, this->win, (GC) this->gc, x, y, width - 1, height - 1 );
}

void
XGraphicsContext::rasterDrawFilledRectangle( int x, int y, unsigned int width, unsigned int height )
{
	Display* dpy = (Display*) this->display;
	XFillRectangle( dpy, this->win, (GC) this->gc, x, y, width, height );
}

void
XGraphicsContext::rasterDrawPolygon( const Sequence& points )
{
	int len = points.getLength();
	XPoint xpoints[len + 1];
	
	for ( int i=0; i < len; i++ )
	{
		const Coordinates& c = dynamic_cast<const Coordinates&>( points.elementAt( i ) );
		xpoints[i].x = c.x;
		xpoints[i].y = c.y;
	}
	
	xpoints[len].x = xpoints[0].x;
	xpoints[len].y = xpoints[0].y;
	
	XDrawLines( (Display*) this->display, this->win, (GC) this->gc, xpoints, len + 1, CoordModeOrigin );
}

void
XGraphicsContext::rasterDrawFilledPolygon( const Sequence& points )
{
	int len = points.getLength();
	XPoint xpoints[len];
	
	for ( int i=0; i < len; i++ )
	{
		const Coordinates& c = dynamic_cast<const Coordinates&>( points.elementAt( i ) );
		xpoints[i].x = c.x;
		xpoints[i].y = c.y;
	}
	
	XFillPolygon( (Display*) this->display, this->win, (GC) this->gc, xpoints, len, 0, CoordModeOrigin );
}

void
XGraphicsContext::rasterDrawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	Display* dpy = (Display*) this->display;
	XDrawArc( dpy, this->win, (GC) this->gc, x, y, width, height, angle1, angle2 );
}

void
XGraphicsContext::rasterDrawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
	Display* dpy = (Display*) this->display;
	XFillArc( dpy, this->win, (GC) this->gc, x, y, width, height, angle1, angle2 );
}

void
XGraphicsContext::rasterDrawString( const String& aString, int x, int y )
{
	//fprintf( stderr, "XGraphicsContext::rasterDrawString: %s %i:%i\n", aString.getChars(), x, y );
  XDrawString( (Display*) this->display, this->win, (GC) this->gc,
               x, y, aString.getChars(), aString.getLength() );
}

/*
int
XGraphicsContext::drawBoundedString( const String& aString, const Region& aRegion, flags options )
{
	TextExtents* extents = this->measureTextExtents( aString );
	int height = extents->ascent + extents->descent;
	this->setClip( aRegion.x1, aRegion.y1, aRegion.width, aRegion.height + extents->descent );
	this->drawString( aString, aRegion.x1, aRegion.y2 );
	this->popClip();
	delete extents;
	
	return height;
}
*/

void
XGraphicsContext::rasterDrawImage( const OffscreenImage& anImage, int ax, int ay )
{
	unsigned int w = anImage.getDrawableWidth();
	unsigned int h = anImage.getDrawableHeight();

	this->copyArea( anImage, 0, 0, w, h, ax, ay );
}

void
XGraphicsContext::rasterDrawImage( const openocl::imaging::Image& anImage, int x, int y )
{
	Display* dpy = (Display*) this->display;
	unsigned long win = this->win;
	GC gc = (GC) this->gc;
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
				XColorRGB xcolor( palette.getColor( pixels[r][c] ) );
				XSetForeground( dpy, gc, xcolor.getColor() );
				XDrawPoint( dpy, win, gc, x + c, y + r );
			} catch ( NoSuchElementException* ex ) {
				delete ex;
			}
		}
	}
}


unsigned int
XGraphicsContext::measureTextWidth( const String& aString ) const
{
	XFontStruct* xfs = (XFontStruct*) this->getFont().getFontId();
	return XTextWidth( xfs, aString.getChars(), aString.getLength() );
}

unsigned int
XGraphicsContext::measureTextHeight( const String& aString ) const
{
	XFontStruct* xfs = (XFontStruct*) this->getFont().getFontId();
	int height, direction, ascent, descent;
	int len = aString.getLength();
	
	switch ( len )
	{
	case 0:
		height = 0;
		break;
	default:
		XCharStruct ch;
		XTextExtents( xfs, aString.getChars(), len, &direction, &ascent, &descent, &ch );
		height = ascent + descent;
	}
	return height;
}

TextExtents*
XGraphicsContext::measureTextExtents( const String& aString ) const
{
	XFontStruct* xfs = (XFontStruct*) this->getFont().getFontId();
	TextExtents* ext = new TextExtents();

	int direction, ascent, descent;
	XCharStruct ch;

	XTextExtents( xfs, aString.getChars(), aString.getLength(), &direction, &ascent, &descent, &ch );

	ext->ascent = ascent;
	ext->descent = descent;
	ext->direction = direction;
	ext->width = ch.width;
  
	return ext;
}

void
XGraphicsContext::copyArea( const Drawable& source,
                            unsigned int sourceX,
                            unsigned int sourceY,
                            unsigned int sourceWidth,
                            unsigned int sourceHeight,
                            Drawable& destination,
                            unsigned int destinationX,
                            unsigned int destinationY )
{ 
  Display* dpy = (Display*) this->display;
  unsigned long int src = source.getId();
  unsigned long int dst = destination.getId();

  XCopyArea( dpy, src, dst, (GC) this->gc, sourceX,
                                           sourceY,
                                           sourceWidth,
                                           sourceHeight,
                                           destinationX,
                                           destinationY );
}

void
XGraphicsContext::copyArea( const Drawable& source,
                            unsigned int sourceX,
                            unsigned int sourceY,
                            unsigned int sourceWidth,
                            unsigned int sourceHeight,
                            unsigned int destinationX,
                            unsigned int destinationY )
{
#ifdef DEBUG_MEMORY
	fprintf( stderr, "\tXGC::copyArea: >>> objCount: %lli\n", Object::getObjectCount() );
	{
#endif

	Region destination;
	destination.x1 = destinationX;
	destination.y1 = destinationY;
	destination.width = sourceWidth;
	destination.height = sourceHeight;
	destination.x2 = destination.x1 + destination.width - 1;
	destination.y2 = destination.y1 + destination.height - 1;

	Region clip = this->getClipRegion();

	if ( clip.containsPartOf( destination ) )
	{
		//	Setting the clip again allows us to put another clip object on
		//	the clip stack. Later the act of removing the clip object from
		//	the stack will undo the changes that we have to do now to properly
		//	draw the image.
		
		this->setClip( destination.x1, destination.y1, destination.width, destination.height );
		{
			Display* dpy = (Display*) this->display;
			unsigned long int src = source.getId();
			unsigned long int dst = this->xdrawable->getId();

			const XPixmap* xpm = dynamic_cast<const XPixmap*>( &source );
			unsigned long mask = 0;
			if ( xpm && (mask = xpm->getMask()) )
			{
				//	Here we have to set the clip origin to that of the image so that
				//	the clip mask is correctly aligned to it. If not accounted for this
				//	could mean that we would overwrite pixels that should be outside of
				//	the clip region.
				
				XSetClipOrigin( dpy, (GC) this->gc, destinationX, destinationY );
				XSetClipMask( dpy, (GC) this->gc, xpm->mask );
			} else {
				//	The mask should not be set, however without this I was having problems.
				XSetClipMask( dpy, (GC) this->gc, None );
			}
			//	Inorder to account for what we have done above we need to restrict the
			//	amount of the image we are going to copy.

			int x_offset = clip.x1 - destination.x1;
			int y_offset = clip.y1 - destination.y1;
			
			int x_inset = destination.x2 - clip.x2;
			int y_inset = destination.y2 - clip.y2;
			if ( x_inset < 0 ) x_inset = 0;
			if ( y_inset < 0 ) y_inset = 0;
			
			{
				int src_x = sourceX + x_offset;
				int src_y = sourceY + y_offset;
				int src_w = sourceWidth  - (x_offset + x_inset);
				int src_h = sourceHeight - (y_offset + y_inset);
				int dst_x = destinationX + x_offset;
				int dst_y = destinationY + y_offset;

				//fprintf( stderr, "XGraphicsContext::copyArea: XCopyArea( %i, %i, %i, %i, %i, %i )\n",
				//	src_x, src_y, src_w, src_h, dst_x, dst_y );
				XCopyArea( dpy, src, dst, (GC) this->gc, src_x, src_y, src_w, src_h, dst_x, dst_y );
			}
			if ( xpm->mask )
			{
				XSetClipMask( dpy, (GC) this->gc, None );
			
				//	I would have thought that I should
				//	reset the origin, but it seems to work without it
				//	and did go awry with it.
				//
				//  XSetClipOrigin( dpy, (GC) this->gc, this->clipOriginX, this->clipOriginY );
			}
		}
		this->popClip();
	}

#ifdef DEBUG_MEMORY
	}
	fprintf( stderr, "\tXGC::copyArea: <<< objCount: %lli\n", Object::getObjectCount() );
#endif
}

void
XGraphicsContext::copyAreaFrom(
                            unsigned int sourceX,
                            unsigned int sourceY,
                            unsigned int sourceWidth,
                            unsigned int sourceHeight,
                            Drawable& destination,
                            unsigned int destinationX,
                            unsigned int destinationY )
{ 
  Display* dpy = (Display*) this->display;
  unsigned long int src = this->xdrawable->getId();
  unsigned long int dst = destination.getId();

  XCopyArea( dpy, src, dst, (GC) this->gc, sourceX,
                                           sourceY,
                                           sourceWidth,
                                           sourceHeight,
                                           destinationX,
                                           destinationY );
}


void
XGraphicsContext::copyAreaFromParent( const Drawable& source,
                                      unsigned int sourceX,
                                      unsigned int sourceY,
                                      unsigned int sourceWidth,
                                      unsigned int sourceHeight,
                                      unsigned int destinationX,
                                      unsigned int destinationY )
{
	const XPanel& source_panel = dynamic_cast<const XPanel&>( source );
	if ( &source_panel )
	{
		Display* dpy = (Display*) this->display;

		unsigned long int dst = this->xdrawable->getId();
		unsigned long src = 0;
		const Panel& panel = source_panel.getContainingPanel();
		if ( &panel )
		{
			src = panel.getId();
		} else {
			src = RootWindow( dpy, source_panel.screen );
		}

		switch ( XCopyArea( dpy, src, dst, (GC) this->gc, sourceX, sourceY, sourceWidth, sourceHeight, destinationX, destinationY ) )
		{
		case BadGC:
		case BadMatch:
		case BadValue:
			IO::err().println( "XGrapihcsContext::copyAreaFromParent: failed\n" );
			break;
		};
	}
}

void
XGraphicsContext::clip( ClipArea clip )
{
	//fprintf( stderr, "XGC::clip()\n" );

    XRectangle rectangles[1];
    rectangles[0].x = 0;
    rectangles[0].y = 0;
    rectangles[0].width  = clip.width;
    rectangles[0].height = clip.height;

    XSetClipRectangles( (Display*) this->display, (GC) this->gc,
                        clip.x, clip.y, rectangles, 1, Unsorted );
}

void
XGraphicsContext::unclip( ClipArea clip )
{
	//fprintf( stderr, "XGC::unclip()\n" );
	//	This should be identical to clip()

    XRectangle rectangles[1];
    rectangles[0].x = 0;
    rectangles[0].y = 0;
    rectangles[0].width  = clip.width;
    rectangles[0].height = clip.height;

    XSetClipRectangles( (Display*) this->display, (GC) this->gc,
                        clip.x, clip.y, rectangles, 1, Unsorted );
}

//unsigned int
//XGraphicsContext::getWidth()
//{
//  return this->width;
//}

//unsigned int
//XGraphicsContext::getHeight()
//{
//  return this->height;
//}

//unsigned int
//XGraphicsContext::getBorderWidth()
//{
//  return this->borderWidth;
//}

/*
unsigned int
XGraphicsContext::getClipOriginX() const
{
  return this->clipOriginX;
}

unsigned int
XGraphicsContext::getClipOriginY() const
{
  return this->clipOriginY;
}

unsigned int
XGraphicsContext::getClipWidth() const
{
  return this->clipWidth;
}

unsigned int
XGraphicsContext::getClipHeight() const
{
  return this->clipHeight;
}
*/

//const openocl::ui::Font&
//XGraphicsContext::getActiveFont() const
//{
//  return *this->activeFont;
//}

FontManager&
XGraphicsContext::getFontManager() const
{
	return this->xdrawable->getVisual().getFontManager();
}

openocl::ui::Visual&
XGraphicsContext::getVisual() const
{
	return const_cast<Visual&>( this->xdrawable->getVisual() );
}

openocl::ui::Drawable&
XGraphicsContext::getDrawable() const
{
	return *this->xdrawable;
}

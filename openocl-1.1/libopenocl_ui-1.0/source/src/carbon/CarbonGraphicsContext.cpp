/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/UIException.h"
#include "openocl/ui/carbon/CarbonGraphicsContext.h"
#include "openocl/ui/carbon/CarbonPanel.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Stack.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <carbon/Carbon.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;
using namespace openocl::ui::carbon;

CarbonGraphicsContext::CarbonGraphicsContext( Drawable& aDrawable )
{
	WindowRef window_ref = (WindowRef) aDrawable.getId();
	
	this->drawable = &aDrawable;
	this->winId = (void*) window_ref;
	this->contentId = null;
	
	CarbonPanel* cpanel = dynamic_cast<CarbonPanel*>( &aDrawable );
	if ( cpanel )
	{
		this->contentId = cpanel->contentId;
	}
}

CarbonGraphicsContext::~CarbonGraphicsContext()
{}

void
CarbonGraphicsContext::clear()
{
}

void
CarbonGraphicsContext::flush()
{
	CGContextFlush( (CGContextRef) this->cgc );
}

void
CarbonGraphicsContext::setDrawable( Drawable& aDrawable )
{
}

void
CarbonGraphicsContext::setForeground( const Color& aColor )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	float red   = aColor.getRed() / 255.0;
	float green = aColor.getGreen() / 255.0;
	float blue  = aColor.getBlue() / 255.0;

	float color_components[] = { red, green, blue, 1.0 };

	fprintf( stderr, "CarbonGraphicsContext::setForeground( (%f,%f,%f) )\n", red, green, blue );
	CGColorSpaceRef color_space = CGColorSpaceCreateWithName( kCGColorSpaceUserRGB );

	CGContextSetStrokeColorSpace( gc, color_space );
	CGColorRef color = CGColorCreate( color_space, color_components );
	CGContextSetStrokeColorWithColor( gc, color );

	CGColorSpaceRelease( color_space );

	//CGContextSetRGBStrokeColor( gc, red, green, blue, 1.0 ); 
}

void
CarbonGraphicsContext::setFill( const Color& aColor )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	float red   = aColor.getRed() / 255.0;
	float green = aColor.getGreen() / 255.0;
	float blue  = aColor.getBlue() / 255.0;

	float color_components[] = { red, green, blue, 1.0 };

	CGColorSpaceRef color_space = CGColorSpaceCreateWithName( kCGColorSpaceUserRGB );

	CGContextSetFillColorSpace( gc, color_space );
	CGColorRef color = CGColorCreate( color_space, color_components );
	CGContextSetFillColorWithColor( gc, color );

	CGColorSpaceRelease( color_space );
}

void
CarbonGraphicsContext::setBackground( const Color& aColor )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	float red   = aColor.getRed() / 255.0;
	float green = aColor.getGreen() / 255.0;
	float blue  = aColor.getBlue() / 255.0;

	float color_components[] = { red, green, blue, 1.0 };

	CGColorSpaceRef color_space = CGColorSpaceCreateWithName( kCGColorSpaceUserRGB );

	CGContextSetFillColorSpace( gc, color_space );
	CGColorRef color = CGColorCreate( color_space, color_components );
	CGContextSetFillColorWithColor( gc, color );

	CGColorSpaceRelease( color_space );
}

void
CarbonGraphicsContext::setFillStyle( mask style )
{
}

void
CarbonGraphicsContext::setFillRule( mask rule )
{
}

openocl::ui::OffscreenImage*
CarbonGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight )
const
{
	return null;
}

openocl::ui::OffscreenImage*
CarbonGraphicsContext::createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const
{
	return null;
}

openocl::ui::OffscreenImage*
CarbonGraphicsContext::createOffscreenImage( const Image& anImage ) const
{
	return null;
}

void
CarbonGraphicsContext::setLineAttributes( unsigned int width,
                                     int          lineStyle,
                                     int          capStyle,
                                     int          joinStyle )
{
}

void
CarbonGraphicsContext::setLineDashes( unsigned int dashOffset,
                                 char dashList[],
                                 unsigned int n )
{
}

void
CarbonGraphicsContext::drawPoint( int x, int y )
{
}

void
CarbonGraphicsContext::drawLine( int x1, int y1, int x2, int y2 )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	if ( gc )
	{
		CGContextBeginPath( gc );
		CGContextMoveToPoint( gc, x1, y1 );
		CGContextAddLineToPoint( gc, x2, y2 );
		CGContextStrokePath( gc );
		CGContextClosePath( gc );
	}
}

void
CarbonGraphicsContext::drawArrowLine( int x1, int y1, int x2, int y2 )
{
}

void
CarbonGraphicsContext::drawRectangle( int x, int y, unsigned int width, unsigned int height )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	if ( gc )
	{
		int w = width;
		int h = height;
		int x2 = (x + w) - 1;
		int y2 = (y + h) - 1;
		
		fprintf( stderr, " %i %i %i %i\n", x, y, x2, y2 );
		
		CGContextBeginPath( gc );
		CGContextMoveToPoint( gc, x, y );
		CGContextAddLineToPoint( gc, x, y2 );
		CGContextAddLineToPoint( gc, x2, y2 );
		CGContextAddLineToPoint( gc, x2, y );
		CGContextClosePath( gc );
		CGContextStrokePath( gc );
	}
}

void
CarbonGraphicsContext::drawFilledRectangle( int x, int y, unsigned int width, unsigned int height )
{
	int h = height - 1;
	CGContextFillRect ( (CGContextRef) this->cgc, CGRectMake( x, y, width, h ));
}

void
CarbonGraphicsContext::drawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
}

void
CarbonGraphicsContext::drawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 )
{
}

void
CarbonGraphicsContext::drawString( const String& aString, int x, int y )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	if ( gc )
	{
		CGPoint start, end;
	
		CGContextSelectFont( gc, "CourierNewPSMT", 12, kCGEncodingMacRoman );
		CGContextSetTextPosition( gc, x, y );
		start = CGContextGetTextPosition( gc );
		CGContextSetTextDrawingMode( gc, kCGTextFillStroke );
		CGContextShowText( gc, aString.getChars(), aString.getLength() );
		end = CGContextGetTextPosition( gc );
		//int value = (int) (end.x - start.x);
	}
}

void
CarbonGraphicsContext::drawImage( const Image& anImage, int x, int y )
{}

unsigned int
CarbonGraphicsContext::measureTextWidth( const String& aString ) const
{
	unsigned int value = 0;
	CGContextRef gc = (CGContextRef) this->cgc;
	if ( gc )
	{
		CGContextSelectFont( gc, "CourierNewPSMT", 12, kCGEncodingMacRoman );
		CGPoint start = CGContextGetTextPosition( gc );
		CGPoint end;
		CGContextSetTextDrawingMode( gc, kCGTextInvisible );
		CGContextShowTextAtPoint( gc, start.x, start.y, aString.getChars(), aString.getLength() );
		end = CGContextGetTextPosition( gc );
		value = (unsigned int) (end.x - start.x);
	}
	
	return value;
}

unsigned int
CarbonGraphicsContext::measureTextHeight( const String& aString ) const
{
	unsigned int value = 35;
	return value;
}

TextExtents*
CarbonGraphicsContext::measureTextExtents( const String& aString ) const
{
	TextExtents* ext = new TextExtents();
	ext->ascent = 30;
	ext->descent = 5;
	ext->width = this->measureTextWidth( aString );
	return ext;
}

void
CarbonGraphicsContext::copyArea( const Drawable& source,
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
CarbonGraphicsContext::copyArea( const Drawable& source,
                            unsigned int srcX,
                            unsigned int srcY,
                            unsigned int srcWidth,
                            unsigned int srcHeight,
                            unsigned int destX,
                            unsigned int destY )
{
}

void
CarbonGraphicsContext::copyAreaFrom(
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
CarbonGraphicsContext::copyAreaFromParent( const Drawable& source,
                                      unsigned int sourceX,
                                      unsigned int sourceY,
                                      unsigned int sourceWidth,
                                      unsigned int sourceHeight,
                                      unsigned int destinationX,
                                      unsigned int destinationY )
{
}

/**
 *  Sets the current clip, and stores the last on the clip stack.
 */
void
CarbonGraphicsContext::setClip( unsigned int x, unsigned int y,
                          unsigned int width, unsigned int height )
{
}

void
CarbonGraphicsContext::popClip()
{
}


void
CarbonGraphicsContext::resetClip()
{
}

//unsigned int
//CarbonGraphicsContext::getWidth()
//{
//  return this->width;
//}

//unsigned int
//CarbonGraphicsContext::getHeight()
//{
//  return this->height;
//}

//unsigned int
//CarbonGraphicsContext::getBorderWidth()
//{
//  return this->borderWidth;
//}

unsigned int
CarbonGraphicsContext::getClipOriginX() const
{
	return this->clipRegion->x1;
}

unsigned int
CarbonGraphicsContext::getClipOriginY() const
{
	return this->clipRegion->y1;
}

unsigned int
CarbonGraphicsContext::getClipWidth() const
{
	return this->clipRegion->width;
}

unsigned int
CarbonGraphicsContext::getClipHeight() const
{
	return this->clipRegion->height;
}

//const XFont&
//CarbonGraphicsContext::getActiveFont() const
//{
//  return *this->activeFont;
//}

void
CarbonGraphicsContext::initializeDefaultFont()
{
}

void
CarbonGraphicsContext::beginPaint( void* drawEvent )
{
	CGContextRef gc;
	OSStatus status = noErr;
	status = GetEventParameter( (EventRef) drawEvent, kEventParamCGContextRef, typeCGContextRef, NULL, sizeof(CGContextRef), NULL, &gc );
	CGContextSetShouldAntialias( gc, false );
	CGColorSpaceRef color_space = CGColorSpaceCreateWithName( kCGColorSpaceUserRGB );
	CGContextSetStrokeColorSpace( gc, color_space );
	CGContextSetFillColorSpace( gc, color_space );
	CGColorSpaceRelease( color_space );

	//CGContextSetStrokeColor( Color::BLACK );
	//CGContextSetFillColor( Color::BLACK );
	
	this->cgc = (void*) gc;
}
	
void
CarbonGraphicsContext::endPaint( void* drawEvent )
{
	CGContextRef gc = (CGContextRef) this->cgc;
	if ( gc )
	{
		CGContextSynchronize( gc );
		CGContextFlush( gc );
		this->cgc = null;
	}
}





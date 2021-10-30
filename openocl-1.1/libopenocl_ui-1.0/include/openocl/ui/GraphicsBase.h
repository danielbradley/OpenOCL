/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_GRAPHICSBASE_H
#define OPENOCL_UI_GRAPHICSBASE_H

#include "openocl/ui.h"
#include "openocl/ui/GraphicsContext.h"
#include <openocl/base/Object.h>
#include <openocl/base.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
  namespace ui {

class ClipArea : public openocl::base::Object
{
public:
	ClipArea( int x = 0, int y = 0, int width = 0xFFFF, int height = 0xFFFF ) : Object()
	{
		this->x = x;
		this->y = y;
		this->width = width;
		this->height = height;
	}

	void setTo( const ClipArea& aClipArea )
	{
		this->x      = aClipArea.x;
		this->y      = aClipArea.y;
		this->width  = aClipArea.width;
		this->height = aClipArea.height;
	}
	
	int x;
	int y;
	int width;
	int height;
};

class GraphicsBase : public openocl::base::Object, public GraphicsContext
{
public:
	GraphicsBase();
	virtual ~GraphicsBase();

	virtual void drawPoint( int x, int y );
	virtual void drawLine( int x1, int y1, int x2, int y2 );
	virtual double drawCurve( int x1, int y1, int x2, int y2, int deviation );
	virtual void drawCurve1( int x1, int y1, int x2, int y2, int deviation );
	virtual void drawCircle( int x, int y, int radius );
	virtual void drawFilledCircle( int x, int y, int radius );
	virtual void drawArrowLine( int x1, int y1, int x2, int y2 );
	virtual void drawArrowHead( int x1, int y1, int x2, int y2, double size );
	virtual void drawArrowHead( int x1, int y1, double degrees, double size );
	virtual void drawRectangle( int x, int y, unsigned int width, unsigned int height );
	virtual void drawRectangleRegion( const Region& aRegion );
	virtual void drawFilledRectangle( int x, int y, unsigned int width, unsigned int height );
	virtual void drawFilledRectangleRegion( const Region& aRegion );
	virtual void drawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void drawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void drawPolygon( const openocl::util::Sequence& points );
	virtual void drawFilledPolygon( const openocl::util::Sequence& points );
	virtual void drawString( const openocl::base::String& aString, int x, int y );
	virtual int  drawBoundedString( const openocl::base::String& aString, const Region& aRegion, flags options );
	virtual void drawImage( const openocl::ui::OffscreenImage& anImage, int x, int y );
	virtual void drawImage( const openocl::imaging::Image& anImage, int x, int y );

	virtual void setScale( double scale );
	virtual void setTranslation( int x=0, int y=0, int z=0 );
	virtual void translate( int x=0, int y=0, int z=0 );
	virtual void setClip( int x, int y, int width, int height );
	virtual void popClip();
	virtual void resetClip();

	virtual void setBrush( const openocl::imaging::Color& color );
	virtual void setFill( const openocl::imaging::Color& color );
	virtual void setBackground( const openocl::imaging::Color& color );

	virtual void setFont( const Font& font ) const;

	//	public constant methods
	virtual Region getClipRegion() const;
	virtual bool canDraw( Region aRegion ) const;

	virtual const openocl::imaging::Color& getBrush() const;
	virtual const openocl::imaging::Color& getFill() const;
	virtual const openocl::imaging::Color& getBackground() const;

	virtual const Font& getFont() const;
	void debugPrintClip() const;

protected:
	virtual void clip( ClipArea clip ) = 0;
	virtual void unclip( ClipArea clip ) = 0;

private:
	int drawVisibleString( const openocl::base::String& string, const Region& box, int offset, int start, int end );
	inline void rescaleAndTranslate( Region& region ) const;
	inline void rescaleAndTranslate( int* x, int* y );
	inline void rescaleAndTranslate( double* x, double* y );
	inline void rescaleAndTranslate( openocl::imaging::Point& p );
	inline void rescale( int* x, int* y );

	openocl::util::Stack* clipStack;
	ClipArea              clipArea;

	int translateX;
	int translateY;
	int translateZ;
	double scale;

	openocl::imaging::Color* brush;
	openocl::imaging::Color* fill;
	openocl::imaging::Color* background;
	const Font* currentFont;
};

};};

#endif

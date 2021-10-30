/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_GRAPHICSCONTEXT_H
#define OPENOCL_UI_GRAPHICSCONTEXT_H

#include <openocl/base/Interface.h>
#include <openocl/base.h>
#include <openocl/imaging.h>
#include "openocl/util.h"
#include "openocl/ui.h"

namespace openocl {
  namespace ui {

/**
 *  The graphics context is used in two scenarios.
 *
 *  First during the layout of a component structure it is used to work out
 *  what sizes things should be using that graphics context. <p>
 *
 *  Then during drawing, the graphics context is again used to actually
 *  draw the components. During drawing the context is clipped using
 *  setClip to restrict where components can draw to. <p>
 *
 *  During, layout there should not be any reason to try and find out the
 *  dimensions of the graphics context or of the graphics context's clip
 *  region. As during layout the gc is passed as a const object, to stop
 *  people accessing these accessors they have been made non-const methods. <p>
 */
class GraphicsContext : public openocl::base::Interface
{
public:
	static const int SOLID_LINE   = 0x0001;
	static const int DASHED_LINE  = 0x0002;
	
	static const int ROUND_ENDCAP  = 0x0100;
	static const int SQUARE_ENDCAP = 0x0200;
	static const int FLAT_ENDCAP   = 0x0400;

	static const int BEVEL_JOIN = 0x1000;
    static const int MITER_JOIN = 0x2000;
	static const int ROUND_JOIN = 0x4000;

	virtual ~GraphicsContext();

	//	public abstract methods (GraphicsContext)
	virtual void setDrawable( Drawable& aDrawable ) = 0;
	virtual void clear() = 0;
	virtual void flush() = 0;

	virtual void setScale( double scale ) = 0;
	//	Should this be called translate instead???
	virtual void setTranslation( int x=0, int y=0, int z=0 ) = 0;
	virtual void translate( int x=0, int y=0, int z=0 ) = 0;

//
//	Drawing methods
//
	//
	//	drawPoint
	//	rasterDrawPoint
	//
	virtual void drawPoint( int x, int y ) = 0;
	virtual void drawLine( int x1, int y1, int x2, int y2 ) = 0;
	virtual double drawCurve( int x1, int y1, int x2, int y2, int deviation ) = 0;  
	virtual void drawCircle( int x, int y, int radius ) = 0;  
	virtual void drawFilledCircle( int x, int y, int radius ) = 0;  
	virtual void drawArrowLine( int x1, int y1, int x2, int y2 ) = 0;
	virtual void drawArrowHead( int x1, int y1, int x2, int y2, double size ) = 0;
	virtual void drawArrowHead( int x1, int y1, double degrees, double size ) = 0;
	virtual void drawRectangle( int x, int y, unsigned int width, unsigned int height ) = 0;
	virtual void drawRectangleRegion( const Region& aRegion ) = 0;

	virtual void drawFilledRectangle( int x, int y, unsigned int width, unsigned int height ) = 0;
	virtual void drawFilledRectangleRegion( const Region& aRegion ) = 0;
	virtual void drawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 ) = 0;
	virtual void drawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 ) = 0;
	virtual void drawPolygon( const openocl::util::Sequence& points ) = 0;
	virtual void drawFilledPolygon( const openocl::util::Sequence& points ) = 0;
	virtual void drawString( const openocl::base::String& aString, int x, int y ) = 0;
	virtual int  drawBoundedString( const openocl::base::String& aString, const Region& aRegion, flags options = 0 ) = 0;
	virtual void drawImage( const openocl::ui::OffscreenImage& anImage, int x, int y ) = 0;
	virtual void drawImage( const openocl::imaging::Image& anImage, int x, int y ) = 0;

	virtual void rasterDrawPoint( int x, int y ) = 0;
	virtual void rasterDrawLine( int x1, int y1, int x2, int y2 ) = 0;
	virtual void rasterDrawRectangle( int x, int y, unsigned int width, unsigned int height ) = 0;
	virtual void rasterDrawFilledRectangle( int x, int y, unsigned int width, unsigned int height ) = 0;
	virtual void rasterDrawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 ) = 0;
	virtual void rasterDrawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 ) = 0;
	virtual void rasterDrawPolygon( const openocl::util::Sequence& points ) = 0;
	virtual void rasterDrawFilledPolygon( const openocl::util::Sequence& points ) = 0;
	virtual void rasterDrawString( const openocl::base::String& aString, int x, int y ) = 0;
	virtual void rasterDrawImage( const openocl::ui::OffscreenImage& anImage, int x, int y ) = 0;
	virtual void rasterDrawImage( const openocl::imaging::Image& anImage, int x, int y ) = 0;

	virtual void setForeground( const openocl::imaging::Color& foreground ) = 0;
	virtual void setFill( const openocl::imaging::Color& fillColor ) = 0; 
	virtual void setBackground( const openocl::imaging::Color& background ) = 0;
	virtual void setFillStyle( mask style ) = 0;
	virtual void setFillRule( mask rule ) = 0;

	virtual void resetLineAttributes() = 0;
	virtual void setLineAttributes( unsigned int width,
                                    int          lineStyle,
                                    int          capStyle,
                                    int          joinStyle ) = 0;

	virtual void setLineDashes( unsigned int dashOffset,
                                char         dashList[],
                                unsigned int n ) = 0;

	virtual void setDefaultFont() const = 0;
	virtual void setFont( const Font& font ) const = 0;

	virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight ) const = 0;
	virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const = 0;
	virtual OffscreenImage* createOffscreenImage( const openocl::imaging::Image& anImage ) const = 0;

	virtual unsigned int measureTextWidth( const openocl::base::String& aString ) const = 0;
	virtual unsigned int measureTextHeight( const openocl::base::String& aString ) const = 0;
	virtual openocl::imaging::TextExtents* measureTextExtents( const openocl::base::String& aString ) const = 0;

  virtual void copyArea( const Drawable& source,
                         unsigned int sourceX,
                         unsigned int sourceY,
                         unsigned int sourceWidth,
                         unsigned int sourceHeight,
                         unsigned int destinationX,
                         unsigned int destinationY ) = 0;

	virtual void copyAreaFrom(
				unsigned int sourceX,
				unsigned int sourceY,
				unsigned int sourceWidth,
				unsigned int sourceHeight,
				Drawable& destination,
				unsigned int destinationX,
				unsigned int destinationY ) = 0;

	virtual void copyAreaFromParent( const Drawable& source,
                                         unsigned int sourceX,
                                         unsigned int sourceY,
                                         unsigned int sourceWidth,
                                         unsigned int sourceHeight,
                                         unsigned int destinationX,
                                         unsigned int destinationY ) = 0;




  virtual void setClip( int x, int y, int width, int height ) = 0;
  virtual void popClip() = 0;
  virtual void resetClip() = 0;

  //virtual unsigned int getWidth() = 0;
  //virtual unsigned int getHeight() = 0;

  virtual Region getClipRegion() const = 0;
  virtual bool canDraw( Region aRegion ) const = 0;
  //virtual unsigned int getClipOriginY() const = 0;
  //virtual unsigned int getClipWidth() const = 0;
  //virtual unsigned int getClipHeight() const = 0;

  virtual const openocl::imaging::Color& getBrush() const = 0;
  virtual const openocl::imaging::Color& getFill() const = 0;
  virtual const openocl::imaging::Color& getBackground() const = 0;

  virtual FontManager& getFontManager() const = 0;
  virtual Visual&      getVisual() const = 0;
  virtual Drawable&    getDrawable() const = 0;
//  const XFont& getActiveFont() const;

  virtual void debugPrintClip() const = 0;


};

};};

#endif

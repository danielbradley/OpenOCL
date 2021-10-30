/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_GRAPHICSCONTEXT_H
#define OPENOCL_UI_X11_GRAPHICSCONTEXT_H

#include <openocl/base/Object.h>
#include "openocl/ui/GraphicsBase.h"
#include "openocl/ui/GraphicsContext.h"
#include <openocl/base.h>
#include <openocl/imaging.h>
#include "openocl/util.h"
#include "openocl/ui.h"
#include "openocl/ui/X11.h"

namespace openocl {
  namespace ui {
    namespace X11 {

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
class XGraphicsContext : public GraphicsBase//,
                         //public openocl::ui::GraphicsContext
{
friend class XPanel;
friend class XPixmap;
public:
	static const int TRUE_COLOR = 0;
	static const int INDEXED    = 1;

	XGraphicsContext( Drawable& anXDrawable );
	virtual ~XGraphicsContext();

	virtual void clear();
	virtual void flush();

	virtual void rasterDrawPoint( int x, int y );
	virtual void rasterDrawLine( int x1, int y1, int x2, int y2 );
	virtual void rasterDrawRectangle( int x, int y, unsigned int width, unsigned int height );
	virtual void rasterDrawFilledRectangle( int x, int y, unsigned int width, unsigned int height );

	virtual void rasterDrawPolygon( const openocl::util::Sequence& points );
	virtual void rasterDrawFilledPolygon( const openocl::util::Sequence& points );

	virtual void rasterDrawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void rasterDrawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void rasterDrawString( const openocl::base::String& aString, int x, int y );
	virtual void rasterDrawImage( const openocl::imaging::Image& anImage, int x, int y );
	virtual void rasterDrawImage( const openocl::ui::OffscreenImage& anImage, int x, int y );


	virtual void setForeground( const openocl::imaging::Color& foreground );
	virtual void setFill( const openocl::imaging::Color& fillColor );
	virtual void setBackground( const openocl::imaging::Color& background );
	virtual void setFillStyle( mask style );
	virtual void setFillRule( mask rule );

	virtual void resetLineAttributes();
	virtual void setLineAttributes( unsigned int width,
                                        int          lineStyle,
                                        int          capStyle,   
                                        int          joinStyle );

	virtual void setLineDashes( unsigned int dashOffset,
                                    char         dashList[],
                                    unsigned int n );

	virtual void setDefaultFont() const;

	virtual void setFont( const Font& font ) const;

	void setDrawable( Drawable& aDrawable );

//-----------------------------------------------------------------
//	public methods
//-----------------------------------------------------------------

//	void setFont( const Font& font );

//-----------------------------------------------------------------
//	public virtual constant methods
//-----------------------------------------------------------------

	virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight ) const;
	virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const;
	virtual OffscreenImage* createOffscreenImage( const openocl::imaging::Image& anImage ) const;

	virtual unsigned int measureTextWidth( const openocl::base::String& aString ) const;
	virtual unsigned int measureTextHeight( const openocl::base::String& aString ) const;

	virtual openocl::imaging::TextExtents* measureTextExtents( const openocl::base::String& aString ) const;

	virtual void copyArea( const Drawable& source,
                               unsigned int sourceX,
                               unsigned int sourceY,
                               unsigned int sourceWidth,
                               unsigned int sourceHeight,
                               Drawable& destination,
                               unsigned int destinationX,
                               unsigned int destinationY );

	virtual void copyArea( const Drawable& source,
                               unsigned int sourceX,
                               unsigned int sourceY,
                               unsigned int sourceWidth,
                               unsigned int sourceHeight,
                               unsigned int destinationX,
                               unsigned int destinationY );

	virtual void copyAreaFrom(
				unsigned int sourceX,
				unsigned int sourceY,
				unsigned int sourceWidth,
				unsigned int sourceHeight,
				Drawable& destination,
				unsigned int destinationX,
				unsigned int destinationY );

	virtual void copyAreaFromParent( const Drawable& source,
                                         unsigned int sourceX,
                                         unsigned int sourceY,
                                         unsigned int sourceWidth,
                                         unsigned int sourceHeight,
                                         unsigned int destinationX,
                                         unsigned int destinationY );

	void clip( ClipArea clip );
	void unclip( ClipArea clip );

//  unsigned int getWidth();
//  unsigned int getHeight();
//  unsigned int getBorderWidth();

	//virtual unsigned int getClipOriginX() const;
	//virtual unsigned int getClipOriginY() const;
	//virtual unsigned int getClipWidth() const;
	//virtual unsigned int getClipHeight() const;

	FontManager& getFontManager() const;
	Visual&      getVisual() const;
	Drawable&    getDrawable() const;

private:
	void initializeDefaultFont();

	Drawable* xdrawable;
	//  const XPanel* panel;

	void* display;
	long unsigned int win;
	void* gc;

	//  unsigned int originX;      //  The origin of the window.
	//  unsigned int originY;      //
	//  unsigned int width;        //  The width and height of the window.
	//  unsigned int height;       //
	//  unsigned int borderWidth;
	//  unsigned int depth;

	//unsigned int clipOriginX;
	//unsigned int clipOriginY;
	//unsigned int clipWidth;
	//unsigned int clipHeight;

	//openocl::util::Stack* clipStack;
};

};};};

#endif

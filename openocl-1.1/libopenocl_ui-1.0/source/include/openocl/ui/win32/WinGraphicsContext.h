/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINGRAPHICSCONTEXT_H
#define OPENOCL_UI_WIN32_WINGRAPHICSCONTEXT_H

#include "openocl/ui/GraphicsBase.h"
#include "openocl/ui.h"
#include "openocl/ui/win32.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class LineAttributes : public openocl::base::Object
{
public:
	unsigned int width;
	int          lineStyle;
	int          capStyle;
	int          joinStyle;
};

class WinGraphicsContext : public openocl::ui::GraphicsBase
{
friend class WinPanel;
friend class WinPixmap;
public:
	WinGraphicsContext( Drawable& anXDrawable );
	explicit WinGraphicsContext( const WinGraphicsContext& aDrawable );
	~WinGraphicsContext();

	//	public virtual methods (GraphicsContext)
	virtual void clear();
	virtual void flush();

	virtual void rasterDrawLine( int x1, int y1, int x2, int y2 );
	//virtual void rasterDrawArrowLine( int x1, int y1, int x2, int y2 );
	virtual void rasterDrawPoint( int x, int y );
	virtual void rasterDrawRectangle( int x, int y, unsigned int width, unsigned int height );
	virtual void rasterDrawFilledRectangle( int x, int y, unsigned int width, unsigned int height );

	virtual void rasterDrawPolygon( const openocl::util::Sequence& points );
	virtual void rasterDrawFilledPolygon( const openocl::util::Sequence& points );

	virtual void rasterDrawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void rasterDrawArcFromCoords( Region box, int x1, int y1, int x2, int y2 );
	virtual void rasterDrawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void rasterDrawString( const openocl::base::String& aString, int x, int y );
	//virtual void drawBoundedString( const openocl::base::String& aString, const Region& aRegion, flags options );
	virtual void rasterDrawImage( const openocl::ui::OffscreenImage& anImage, int x, int y );
	virtual void rasterDrawImage( const openocl::imaging::Image& anImage, int x, int y );

	virtual void setDrawable( Drawable& aDrawable );
	virtual void setForeground( const openocl::imaging::Color& foreground );
	virtual void setFill( const openocl::imaging::Color& fillColor );
	virtual void setBackground( const openocl::imaging::Color& background );

	virtual void setFillStyle( mask style );
	virtual void setFillRule( mask rule );

	virtual void setDefaultFont() const;
	virtual void setFont( const Font& font ) const;

  virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight ) const;
  virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const;
  virtual OffscreenImage* createOffscreenImage( const openocl::imaging::Image& anImage ) const;

  virtual void resetLineAttributes();
  virtual void setLineAttributes( unsigned int width,
                                  int          lineStyle,
                                  int          capStyle,   
                                  int          joinStyle );

  virtual void setLineDashes( unsigned int dashOffset,
                              char         dashList[],
                              unsigned int n );


  unsigned int measureTextWidth( const openocl::base::String& aString ) const;
  unsigned int measureTextHeight( const openocl::base::String& aString ) const;

  openocl::imaging::TextExtents*
  measureTextExtents( const openocl::base::String& aString ) const;

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


  //void setClip( unsigned int x, unsigned int y,
  //              unsigned int width, unsigned int height );

  //void popClip();

  //void resetClip();

	virtual void clip( ClipArea clip );
	virtual void unclip( ClipArea clip );

//  unsigned int getWidth();
//  unsigned int getHeight();
//  unsigned int getBorderWidth();

  virtual unsigned int getClipOriginX() const;
  virtual unsigned int getClipOriginY() const;
  virtual unsigned int getClipWidth() const;
  virtual unsigned int getClipHeight() const;

  virtual FontManager& getFontManager() const;
  virtual Visual& getVisual() const;
  virtual Drawable& getDrawable() const;

//  const XFont& getActiveFont() const;

private:
	void initializeDefaultFont();
	void winRefreshPen();
	void winRefreshBrush();

	void beginPaint( void* paintStruct ) const;
	void endPaint( void* paintStruct ) const;

	//	private members
	Drawable* drawable;			//	Either a WinPanel or WinPixmap
	unsigned long winId;		//	Windows DDC
	void*	ddc;
	void*   paintStruct;
	int		ddcUsers;


	//Region*               clipRegion;
	//openocl::util::Stack* clipStack;

	void* pen;
	void* brush;

	const Font* activeFont;

	long foreground;
	long background;
	long fill;

	LineAttributes lineAttributes;
};

};};};

#endif

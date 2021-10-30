/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CARBON_CARBONGRAPHICSCONTEXT_H
#define OPENOCL_UI_CARBON_CARBONGRAPHICSCONTEXT_H

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui.h"
#include "openocl/ui/carbon.h"
#include <openocl/base/Object.h>
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {
		namespace carbon {

class CarbonGraphicsContext
: public openocl::base::Object, public openocl::ui::GraphicsContext
{
public:
	CarbonGraphicsContext( openocl::ui::Drawable& aDrawable );
	virtual ~CarbonGraphicsContext();

	//	public virtual methods (GraphicsContext)
	virtual void clear();
	virtual void flush();

	virtual void setDrawable( Drawable& aDrawable );
	virtual void setForeground( const openocl::imaging::Color& foreground );
	virtual void setFill( const openocl::imaging::Color& foreground );
	virtual void setBackground( const openocl::imaging::Color& background );

	virtual void setFillStyle( mask style );
	virtual void setFillRule( mask rule );

//  void setFont( XFont* font );

  virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight ) const;
  virtual OffscreenImage* createOffscreenImage( unsigned int aWidth, unsigned int aHeight, unsigned int aDepth ) const;
  virtual OffscreenImage* createOffscreenImage( const openocl::imaging::Image& anImage ) const;
  
  virtual void setLineAttributes( unsigned int width,
                                  int          lineStyle,
                                  int          capStyle,   
                                  int          joinStyle );

  virtual void setLineDashes( unsigned int dashOffset,
                              char         dashList[],
                              unsigned int n );

  virtual void drawLine( int x1, int y1, int x2, int y2 );
  virtual void drawArrowLine( int x1, int y1, int x2, int y2 );
  virtual void drawPoint( int x, int y );
  virtual void drawRectangle( int x, int y, unsigned int width, unsigned int height );
  virtual void drawFilledRectangle( int x, int y, unsigned int width, unsigned int height );
  virtual void drawArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
  virtual void drawFilledArc( int x, int y, unsigned int width, unsigned int height, int angle1, int angle2 );
	virtual void drawString( const openocl::base::String& aString, int x, int y );
	virtual void drawImage( const openocl::imaging::Image& anImage, int x, int y );

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


  void setClip( unsigned int x, unsigned int y,
                unsigned int width, unsigned int height );

  void popClip();

  void resetClip();

//  unsigned int getWidth();
//  unsigned int getHeight();
//  unsigned int getBorderWidth();

  virtual unsigned int getClipOriginX() const;
  virtual unsigned int getClipOriginY() const;
  virtual unsigned int getClipWidth() const;
  virtual unsigned int getClipHeight() const;

//  const XFont& getActiveFont() const;

	void beginPaint( void* drawEvent );
	void endPaint( void* drawEvent );
private:

	void initializeDefaultFont();

	//	private members
	Drawable*	drawable;	//	Either a CarbonPanel or CarbonPixmap
	void*		winId;		//	Window Reference
	void*		contentId;		//	HIView Refernece

	//	transient private members
	void*	cgc;				//	Carbon CGContextRef
	Region* region;

	Region*               clipRegion;
	openocl::util::Stack* clipStack;

	void* pen;
	void* brush;

//  XFont* activeFont;

	
};

};};};

#endif

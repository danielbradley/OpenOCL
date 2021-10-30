/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINPIXMAP_H
#define OPENOCL_UI_WIN32_WINPIXMAP_H

#include "openocl/ui/Drawable.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/win32.h"
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class WinPixmap : public openocl::ui::OffscreenImage
{
friend class WinGraphicsContext;
public:
	WinPixmap( const Drawable& aDrawable, unsigned int aWidth,
                                          unsigned int aHeight,
                                          unsigned int aDepth );
	WinPixmap( const WinPanel& anWinPanel, const openocl::imaging::Image& anImage );
	virtual ~WinPixmap();

	void clear();
	void import( const openocl::imaging::Image& anImage );

  /**
   *  If you wanted to copy the entire of pixmap to a location of
   *  panel you would to
   *
   *   panel->copyArea( pixmap, 0, 0,
   *                            pixmap->getDrawableWidth(),
   *                            pixmap->getDrawableHeight(),
   *                            30, 40 );
   */
  void copyArea( const Drawable& aDrawable,
                 unsigned int    sourceX,
                 unsigned int    sourceW,
                 unsigned int    sourceWidth,
                 unsigned int    sourceHeight,
                 unsigned int    destinationX,
                 unsigned int    destinationY ) const;

  virtual unsigned int getDrawableWidth() const;
  virtual unsigned int getDrawableHeight() const;
  virtual unsigned int getDrawableDepth() const;

  virtual const Visual& getVisual() const;
  virtual const WinVisual& getWinVisual() const;
  virtual unsigned long getId() const;
  virtual unsigned long getScreenId() const;
  virtual GraphicsContext& getGraphicsContext() const;

	//	public methods
	void* getMask();
	void* getMask() const;

private:
	static int nrOfPixmaps;
	static int sumOfPixmapSizes;

	void* bitmapId;
	void* maskId;

	const Drawable* panel;
	unsigned int screen;

	unsigned int width;
	unsigned int height;
	unsigned int depth;

	WinGraphicsContext* gc;
};

};};};

#endif

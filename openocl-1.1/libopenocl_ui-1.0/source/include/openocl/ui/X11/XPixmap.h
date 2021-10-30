/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XPIXMAP_H
#define OPENOCL_UI_X11_XPIXMAP_H

#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Drawable.h"
#include "openocl/ui/X11.h"
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {
		namespace X11 {

class XPixmap : public openocl::ui::OffscreenImage
{
friend class XGraphicsContext;
public:
	XPixmap( const Drawable& aDrawable, unsigned int aWidth,
                                            unsigned int aHeight,
                                            unsigned int aDepth );
	XPixmap( const XPanel& anXPanel, const openocl::imaging::Image& anImage );
	virtual ~XPixmap();

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
	virtual const XVisual& getXVisual() const;
	virtual unsigned long getId() const;
	virtual unsigned long getScreenId() const;
	virtual GraphicsContext& getGraphicsContext() const;

	//	public constant methods
	unsigned long getMask() const;

private:
	const Drawable& xdrawable;

	void*        xdisplay;
	unsigned int screen;

	unsigned long int pixmap;
	unsigned long int mask;
	unsigned int width;
	unsigned int height;
	unsigned int depth;

	XGraphicsContext* gc;

};

};};};

#endif

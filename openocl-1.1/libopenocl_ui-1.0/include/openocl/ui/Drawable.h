/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DRAWABLE_H
#define OPENOCL_UI_DRAWABLE_H

#include "openocl/ui.h"
#include <openocl/base/Interface.h>

namespace openocl {
  namespace ui {

class Drawable : public openocl::base::Interface
{
public:
  virtual ~Drawable();

  /**
   *  If you wanted to copy the entire of pixmap to a location of
   *  panel you would to
   *
   *   panel->copyArea( pixmap, 0, 0,
   *                            pixmap->getDrawableWidth(),
   *                            pixmap->getDrawableHeight(),
   *                            30, 40 );
  virtual void copyArea( const Drawable& aDrawable,
                         unsigned int sourceX,
                         unsigned int sourceW,
                         unsigned int sourceWidth,
                         unsigned int sourceHeight,
                         unsigned int destinationX,
                         unsigned int destinationY ) const = 0;
   */

	virtual GraphicsContext& getGraphicsContext() const = 0;

	virtual const Visual& getVisual() const = 0;
	virtual unsigned long getId() const = 0;
	virtual unsigned long getScreenId() const = 0;

	virtual unsigned int getDrawableWidth() const = 0;
	virtual unsigned int getDrawableHeight() const = 0;
	virtual unsigned int getDrawableDepth() const = 0;
};

};};

#endif

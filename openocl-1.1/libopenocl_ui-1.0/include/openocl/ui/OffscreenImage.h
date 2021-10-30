/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_OFFSCREENIMAGE_H
#define OPENOCL_UI_OFFSCREENIMAGE_H

#include <openocl/base/Object.h>
#include "openocl/ui/Drawable.h"
#include "openocl/ui.h"

namespace openocl {
  namespace ui {

class OffscreenImage : public openocl::base::Object,
                       public openocl::ui::Drawable
{
public:
  virtual ~OffscreenImage();

  virtual void clear() = 0;
  virtual void copyArea( const Drawable& aDrawable,
                         unsigned int    sourceX,
                         unsigned int    sourceW,
                         unsigned int    sourceWidth,
                         unsigned int    sourceHeight,
                         unsigned int    destinationX,
                         unsigned int    destinationY ) const = 0;

  virtual GraphicsContext& getGraphicsContext() const = 0;
  virtual const Visual& getVisual() const = 0;
  virtual unsigned long int getId() const = 0;

  virtual unsigned int getDrawableWidth() const = 0;
  virtual unsigned int getDrawableHeight() const = 0;
  virtual unsigned int getDrawableDepth() const = 0;


};

};};

#endif

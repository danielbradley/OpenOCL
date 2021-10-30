/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XDRAWABLE_H
#define OPENOCL_UI_X11_XDRAWABLE_H

#include "openocl/ui/Drawable.h"
#include "openocl/ui/X11.h"

namespace openocl {
  namespace ui {
    namespace X11 {

class XDrawable : public openocl::ui::Drawable
{
public:
  virtual ~XDrawable();

  virtual unsigned long int getId() const = 0;
  virtual const XVisual& getXVisual() const = 0;
};

};};};

#endif 

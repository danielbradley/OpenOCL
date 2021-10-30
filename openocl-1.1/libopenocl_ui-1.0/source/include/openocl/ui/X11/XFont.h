/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XFONT_H
#define OPENOCL_UI_X11_XFONT_H

#include "openocl/ui.h"
#include "openocl/ui/X11.h"
#include <openocl/base/Object.h>
#include <openocl/base/Exception.h>
#include <openocl/base.h>

namespace openocl {
	namespace ui {
		namespace X11 {

class XFont : public openocl::base::Object
{
friend class XGraphicsContext;
friend class XDisplay;
public:
	unsigned long getId() const;
private:
	XFont( XVisual& aVisual, void* fontStruct );

	XVisual& xvisual;
	void* fontInfo;
};

};};};

#endif

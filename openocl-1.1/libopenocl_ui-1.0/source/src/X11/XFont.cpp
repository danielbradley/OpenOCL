/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/X11/XFont.h"
#include "openocl/ui/X11/XVisual.h"
#include "openocl/ui/Xincludes.h"

#include <openocl/base/Exception.h>
#include <openocl/base/String.h>

using namespace openocl::base;
using namespace openocl::ui;
using namespace openocl::ui::X11;

XFont::XFont( XVisual& aVisual, void* fontStruct )
: Object(), xvisual( aVisual )
{
	this->fontInfo = fontStruct;
}

unsigned long
XFont::getId() const
{
	XFontStruct* xfs = (XFontStruct*) this->fontInfo;
	return xfs->fid;
}

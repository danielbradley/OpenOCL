/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_FONTMANAGER_H
#define OPENOCL_UI_FONTMANAGER_H

#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/base/event.h>
#include <openocl/base/Interface.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/imaging/FontInfo.h>
#include <openocl/util.h>

namespace openocl {
  namespace ui {

class FontManager : public openocl::base::Interface
{
public:
	FontManager();
	virtual ~FontManager();

	virtual openocl::base::String* loadFont( const openocl::imaging::FontInfo& details ) = 0;
	virtual const Font&            getDefaultFont() const = 0;
	virtual const Font&            findFont( const openocl::base::String& fontId ) const
	                                   throw (openocl::base::NoSuchElementException*) = 0;
};

};};

#endif


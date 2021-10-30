/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINFONTMANAGER_H
#define OPENOCL_UI_WIN32_WINFONTMANAGER_H

#include "openocl/ui/FontManager.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class WinFontManager : public openocl::ui::FontManager
{
public:
	WinFontManager();
	virtual ~WinFontManager();

	virtual openocl::base::String*          loadFont( const openocl::imaging::FontInfo& details );
	virtual const Font& getDefaultFont() const;
	virtual const Font& findFont( const openocl::base::String& fontId ) const
		throw (openocl::base::NoSuchElementException*);
private:
	openocl::util::Dictionary* fonts;
	const Font* defaultFont;
};

};};};

#endif

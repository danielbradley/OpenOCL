/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_FONT_H
#define OPENOCL_UI_FONT_H

#include "openocl/ui.h"
#include <openocl/base/Object.h>
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {

class Font : public openocl::base::Object
{
friend class FontManager;
public:
	virtual ~Font();

	const openocl::imaging::FontInfo& getFontInfo() const;
	void* getFontId() const;

	Font( const openocl::imaging::FontInfo& fontInfo, void* aFontId );
private:

	openocl::imaging::FontInfo* fontInfo;
	void* fontId;
	
};

};};

#endif

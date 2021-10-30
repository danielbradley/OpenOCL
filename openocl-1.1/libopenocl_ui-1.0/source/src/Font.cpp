/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Font.h"
#include <openocl/imaging/FontInfo.h>

using namespace openocl::imaging;
using namespace openocl::ui;

Font::Font( const FontInfo& fontInfo, void* aFontId )
{
	this->fontInfo = new FontInfo( fontInfo );
	this->fontId   = aFontId;
}

Font::~Font()
{
	delete this->fontInfo;
	//  this->fontId;  The information stored by this->fontId should be deleted
	//	when the FontManager terminates by calling getFontId() and deleting it
	//	in the platform specific way.
}

const FontInfo&
Font::getFontInfo() const
{
	return *this->fontInfo;
}

void*
Font::getFontId() const
{
	return this->fontId;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XFONTMANAGER_H
#define OPENOCL_UI_X11_XFONTMANAGER_H

#include "openocl/ui/X11.h"
#include "openocl/ui/FontManager.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {
		namespace X11 {

class XFontManager : public openocl::ui::FontManager
{
public:
	XFontManager( void* xdisplay );
	virtual ~XFontManager();

	virtual openocl::base::String*  loadFont( const openocl::imaging::FontInfo& details );
	virtual const Font&             getDefaultFont() const;
	virtual const Font&             findFont( const openocl::base::String& fontId ) const
	                                    throw (openocl::base::NoSuchElementException*);
private:
	void* XFontManager::loadXFont( const openocl::base::String& fontName );
	openocl::base::String* matchToSystemFontId( const openocl::imaging::FontInfo& fontInfo );

	void*                      xdisplay;
	openocl::util::Dictionary* fonts;
	const Font*                defaultFont;
};

};};};

#endif

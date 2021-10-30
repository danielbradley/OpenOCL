/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/X11/XFontManager.h"
#include "openocl/ui/X11/XVisual.h"
#include "openocl/ui/Font.h"
#include <openocl/base/FormattedString.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/imaging/FontInfo.h>
#include <openocl/util/ADTObject.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/StringKey.h>

#include <cstdio>
#include <X11/Xlib.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::ui::X11;
using namespace openocl::util;

XFontManager::XFontManager( void* xdisplay )
: FontManager(), xdisplay( xdisplay )
{
	this->fonts = new Dictionary();
	
	FontInfo font_info( "clean", 140, 0 );
	//  Changed default font for X Windows // FontInfo font_info( "courier", 140, 0 );

	String* font_id = this->loadFont( font_info );
	this->defaultFont = &this->findFont( *font_id );
	delete font_id;
}

XFontManager::~XFontManager()
{
	Display* dpy = (Display*) this->xdisplay;
	Iterator* it = this->fonts->values();
	while( it->hasNext() )
	{
		const Font& font = dynamic_cast<const Font&>( it->next() );
		XFontStruct* font_struct = reinterpret_cast<XFontStruct*>( font.getFontId() );
		XFreeFont( dpy, font_struct );
	}
	delete it;
	delete this->fonts;
}

String*
XFontManager::loadFont( const FontInfo& fontInfo )
{
	String* font_id = this->matchToSystemFontId( fontInfo );

	void* xfontstruct = this->loadXFont( *font_id );

	if ( null == xfontstruct )
	{
		fprintf( stderr, "XFontManager::loadFont: could not load font!!\n" );
	}

	Font* font = new Font( fontInfo, (void*) xfontstruct );
	StringKey* key = new StringKey( *font_id );
	this->fonts->put( key, font );

	return font_id;
}

const openocl::ui::Font&
XFontManager::getDefaultFont() const
{
	return *this->defaultFont;
}

const openocl::ui::Font&
XFontManager::findFont( const String& fontId ) const
throw (NoSuchElementException*)
{
	StringKey key( fontId );
	const Object& obj = this->fonts->getValue( key );
	return dynamic_cast<const Font&>( obj );
}

//---------------------------------------------------------------------
//	private methods (XFontManager)
//---------------------------------------------------------------------

void*
XFontManager::loadXFont( const String& fontId )
{
	Display* dpy = (Display*) this->xdisplay;
	const char* font_id = fontId.getChars();

	XFontStruct* font = XLoadQueryFont( dpy, font_id );
	return font;
}

String*
XFontManager::matchToSystemFontId( const FontInfo& fontInfo )
{
	const char* style;
	switch ( fontInfo.getCharacterStyle() )
	{
	case FontInfo::BOLD:
		style = "bold";
		break;
	case FontInfo::ITALIC:
		style = "italic";
		break;
	default:
		style = "medium";
	}

	//
	//                                    family weight
	//                                        |  |
	String* font_id = new FormattedString( "*%s-%s-r-*-%i*", fontInfo.getFontName().getChars(), style, fontInfo.getPointSize() );
	//String* font_id = new String( "*clean-medium-r-*-140*" );
	return font_id;
}

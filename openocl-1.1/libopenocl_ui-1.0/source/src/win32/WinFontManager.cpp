/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/Font.h"
#include "openocl/ui/win32/WinFontManager.h"
#include <openocl/base/FormattedString.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/imaging/FontInfo.h>
#include <openocl/util/ADTObject.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/StringKey.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::ui::win32;
using namespace openocl::util;

static const int MAX_FONT_FACE_NAME_LENGTH = 32;

WinFontManager::WinFontManager()
{
	this->fonts = new Dictionary();

	FontInfo fixed( "Arial", 15, 0 );
	String* font_id = this->loadFont( fixed );
	this->defaultFont = &this->findFont( *font_id ); 
	delete font_id;
}

WinFontManager::~WinFontManager()
{
	//	XXX	Need to unload fonts and delete dictionary

	Iterator* it = this->fonts->values();
	while( it->hasNext() )
	{
		const Font& font = dynamic_cast<const Font&>( it->next() );
		HFONT winfont = reinterpret_cast<HFONT>( font.getFontId() );
		DeleteObject( winfont );
	}
	delete it;
	delete this->fonts;
}

String*
WinFontManager::loadFont( const FontInfo& details )
{
	LOGFONT* font = (LOGFONT*) Runtime::calloc( 1, sizeof( LOGFONT ) );
	const String& font_name = details.getFontName();
	unsigned int point_size = details.getPointSize();
	flags style = details.getCharacterStyle();

	font->lfHeight = point_size;
	font->lfWidth = 0;
	font->lfEscapement = 0;
	font->lfOrientation = 0;

	switch ( style )
	{
	case FontInfo::BOLD:
		font->lfWeight = 700;
		break;
	default:
		font->lfWeight = 400;
	}

	if ( FontInfo::ITALIC & style )
	{
		font->lfItalic = TRUE;
	}

	if ( FontInfo::UNDERLINE & style )
	{
		font->lfUnderline = TRUE;
	}

	if ( FontInfo::STRIKEOUT & style )
	{
		font->lfStrikeOut = TRUE;
	}

	font->lfCharSet = DEFAULT_CHARSET;
	font->lfOutPrecision = OUT_DEFAULT_PRECIS;
	font->lfClipPrecision = CLIP_DEFAULT_PRECIS;
	font->lfQuality = ANTIALIASED_QUALITY;
	//font.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	unsigned int max = MAX_FONT_FACE_NAME_LENGTH - 1;
	max = min( max, font_name.getLength() );
	for ( unsigned int i=0; i < max; i++ )
	{
		font->lfFaceName[i] = font_name.charAt( i );
	}
	font->lfFaceName[max] = '\0';

	HFONT win_font = CreateFontIndirect( font );

	String* fontId = new FormattedString( "%s_%i_%x", font_name.getChars(), point_size, style );
	Font* afont = new Font( details, win_font ); 
	this->fonts->put( new StringKey( *fontId ), afont );

	Runtime::free( font );

	return fontId;
}

const Font&
WinFontManager::getDefaultFont() const
{
	return *this->defaultFont;
}

const Font&
WinFontManager::findFont( const String& fontId ) const
throw (NoSuchElementException*)
{
	StringKey key( fontId );
	const Object& obj = this->fonts->getValue( key );
	return dynamic_cast<const Font&>( obj );
}

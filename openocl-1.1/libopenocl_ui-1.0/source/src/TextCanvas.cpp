/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/ScrollBar.h"
#include "openocl/ui/TextCanvas.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/FontInfo.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/ui/Drawable.h>
#include <openocl/ui/FontManager.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/StringTokenizer.h>

#include <stdio.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

TextCanvas::TextCanvas()
{
	this->setPadding( 0 );
	this->textBuffer = new StringBuffer();
	this->margin = DEFAULT_MARGIN;
	this->spacing = DEFAULT_SPACING;
	
	this->fontInfo = null;
	this->fontId = null;
	this->brush = new Color( Color::BLACK );
	this->background = new Color( Color::CANVAS );
}

TextCanvas::~TextCanvas()
{
	delete this->textBuffer;
	delete this->fontInfo;
	delete this->brush;
	delete this->background;
}

void
TextCanvas::setMargin( unsigned int size )
{
	this->margin = size;
}

void
TextCanvas::setSpacing( unsigned int size )
{
	this->spacing = size;
}

void
TextCanvas::setText( const String& text )
{
#ifdef DEBUG_OPENOCL_UI_TEXTCANVAS
	fprintf( stderr, "TextCanvas::setText()\n" );
#endif
	delete this->textBuffer;
	this->textBuffer = new StringBuffer();
	this->textBuffer->append( text );
	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}

void
TextCanvas::appendText( const String& text )
{
#ifdef DEBUG_OPENOCL_UI_TEXTCANVAS
	fprintf( stderr, "TextCanvas::appendText()\n" );
#endif
	this->textBuffer->append( text );
	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}

void
TextCanvas::setFontInfo( const FontInfo& fontInfo )
{
	delete this->fontInfo;
	delete this->fontId;
	this->fontId = null;
	this->fontInfo = new FontInfo( fontInfo );
}

void
TextCanvas::setBrush( const openocl::imaging::Color& aColor )
{
	delete this->brush;
	this->brush = new Color( aColor.getColor() );
}

void
TextCanvas::setBackground( const openocl::imaging::Color& aColor )
{
	delete this->background;
	this->background = new Color( aColor.getColor() );
}

bool
TextCanvas::drawCanvas( GraphicsContext& gc ) const
{
	unsigned int tabWidth = 40;
#ifdef DEBUG_OPENOCL_UI_TEXTCANVAS
	fprintf( stderr, "TextCanvas::drawCanvas()\n" );
#endif

	const Color& previous_bg = gc.getBackground();
	gc.setBackground( *this->background );
		
	if ( this->fontInfo )
	{
		FontManager& fm = this->getContainingPanel().getVisual().getFontManager();

		if ( !this->fontId )
		{
			const_cast<TextCanvas*>( this )->fontId = fm.loadFont( *this->fontInfo );
		}
		const Font& font = fm.findFont( *this->fontId );
		gc.setFont( font );
	}

	int margin  = this->getMargin();
	int spacing = this->getSpacing();

	Region bounds = this->getBounds();

	gc.setForeground( *this->brush );
	gc.setFill( *this->background );
	gc.drawFilledRectangleRegion( bounds );

	if ( this->textBuffer )
	{
		StringTokenizer st( this->textBuffer->asString() );
		st.setDelimiters( "\t\n" );
		gc.setForeground( *this->brush );

		Region box;
		box.x1 = bounds.x1 + margin;
		box.x2 = bounds.x2 - margin;
		box.width = box.x2 - box.x1 + 1;
		box.y1 = bounds.y1 + margin;
		box.y2 = bounds.y2 - margin;
		box.height = box.y2 - box.y1 + 1;

		int y = box.y1;
		
#ifdef DEBUG_OPENOCL_UI_TEXTCANVAS
		fprintf( stderr, "TextCanvas::drawCanvas: %s\n", this->textBuffer->asString().getChars() );
		gc.drawString( this->textBuffer->asString(), box.x1, y );
#endif
		int x = box.x1;

		Sequence* tokens = st.tokenize();
		Iterator* it = tokens->iterator();
		while ( it->hasNext() )
		{
			const String& str = dynamic_cast<const String&>( it->next() );
			TextExtents* extents = gc.measureTextExtents( str );
			char del = str.charAt( 0 );

			switch ( del )
			{
			case '\t':
			case '\n':
				{
					unsigned int max = str.getLength();
					for ( unsigned int i=0; i < max; i++ )
					{
						char del = str.charAt( i );
						switch ( del )
						{
						case '\t':
							x = ((x - box.x1) % tabWidth) + tabWidth;
							break;
						case '\n':
							x = box.x1;
							y += extents->descent;
							y += extents->ascent;
							y += spacing;
							break;
						default:
							;
						}
					}
				}
				break;
			default:
				gc.drawString( str, x, y );
				x += extents->width;
			}
			delete extents;
		}
		delete it;
		delete tokens;
	}
	gc.setBackground( previous_bg );
	
	return false;
}

Dimensions
TextCanvas::getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	unsigned int margin  = this->getMargin();
	unsigned int spacing = this->getSpacing();

	Dimensions d;
	Dimensions text;
	Dimensions box;
	
	StringTokenizer st( this->textBuffer->asString() );
	st.setDelimiter( '\n' );

	{
		TextExtents* extents = gc.measureTextExtents( "Laughter" );
		Sequence* tokens = st.tokenize();
	
		unsigned int line_height = extents->ascent + extents->descent + spacing;
		unsigned int max_width = 0;

		unsigned int max = tokens->getLength();
		for ( unsigned int i=0; i < max; i++ )
		{
			const String& str = dynamic_cast<const String&>( tokens->elementAt( i ) );

			if ( str.contains( "\n" ) )
			{
				text.height += (line_height * str.getLength());
			}
			else
			{
				TextExtents* line_extents = gc.measureTextExtents( str );
				max_width = Math::max( max_width, line_extents->width );
				delete line_extents;
			}
		}
		text.width = max_width;
		text.height += line_height;

		delete tokens;
		delete extents;
	}

	box.width  = text.width  + margin*2;
	box.height = text.height + margin*2;

	d.width  = Math::min( box.width, width );
	d.height = Math::min( box.height, height );

	return d;
}

unsigned int
TextCanvas::getMargin() const
{
	return this->margin;
}

unsigned int
TextCanvas::getSpacing() const
{
	return this->spacing;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/TextArea.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int TextArea::padding = 1;

TextArea::TextArea( const String& aString ) : Textual( aString )
{
	this->setTextLayoutHints( Textual::LEFT );
	this->setMargin( 2 );
	this->setIndent( 0 );
	this->lineSpacing = 2;
	this->text = new String();
	this->lines = new Sequence();
}

TextArea::~TextArea()
{
	delete this->text;
	delete this->lines;
}

//-------------------------------------------------------------------------
//	public virtual methods (Container)
//-------------------------------------------------------------------------

void
TextArea::doLayout( const GraphicsContext& gc )
{
	delete this->lines;
	this->lines = new Sequence();

	unsigned int margin = this->getMargin();

	Region bounds;
	Region outline;
	
	bounds.x1     = this->getX();
	bounds.y1     = this->getY();
	bounds.width  = this->getWidth();
	bounds.height = this->getHeight();
	bounds.x2     = bounds.x1 + bounds.width - 1;
	bounds.y2     = bounds.y1 + bounds.height - 1;
	
	outline.x1 = bounds.x1 + margin;
	outline.x2 = bounds.x2 - margin;
	outline.width = bounds.x2 - bounds.x1 + 1;
	outline.y1 = bounds.y1 + margin;
	outline.y2 = bounds.y2 - margin;
	outline.height = bounds.y2 - bounds.y1 + 1;
	
	unsigned int start = 0;
	unsigned int last = 0;
	unsigned int current = 1;
	unsigned int end = this->text->getLength() - 1;
	
	char c;
	while ( current < end )
	{
		c = this->text->charAt( current );
		switch( c )
		{
		case ' ':
			{
				String* str = this->text->substring( start, current - 1 );
				int text_width = gc.measureTextWidth( *str );
				if ( outline.width < text_width )
				{
					this->lines->add( this->text->substring( start, last - 1 ) );
					start = Math::min( current + 1, end );
					last  = start;
				} else {
					last = Math::min( current + 1, end );
				}
			}
			break;
		}
		current++;
	}
	
	this->lines->add( this->text->substring( start, end ) );
}


//-------------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------------

void
TextArea::setText( const String& text )
{
	delete this->text;
	this->text = text.toString();
}

//-------------------------------------------------------------------------
//	public virtual constant methods (Component)
//-------------------------------------------------------------------------

Dimensions
TextArea::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;

	unsigned int margin = this->getMargin();
	unsigned int indent = this->getIndent();

	int max_text_width = width - (margin*2) - indent;
	
	if ( 0 < max_text_width )
	{
		const_cast<TextArea*>( this )->breakLines( gc, max_text_width );
	}
	
	TextExtents* extents;
	Iterator* it = this->lines->iterator();
	while ( it->hasNext() )
	{
		const String& line = dynamic_cast<const String&>( it->next() );
		extents = gc.measureTextExtents( line );
		dim.height += (extents->ascent + extents->descent + this->lineSpacing);
		dim.width = Math::max( dim.width, extents->width );
		delete extents;
	}
	delete it;
	
	dim.width += (margin*2) + indent + this->lineSpacing;
	dim.height += (margin*2);
	
	return dim;
}

bool
TextArea::draw( GraphicsContext& gc ) const
{
	//flags hints = this->getLayoutHints();
	int x = this->getX();
	int y = this->getY();
	int w = this->getWidth();
	int h = this->getHeight();

	int margin = this->getMargin();
	int indent = this->getIndent();

	Region bounds;
	bounds.width = w;
	bounds.x1 = x;
	bounds.x2 = x + w - 1;
	bounds.height = h;
	bounds.y1 = y;
	bounds.y2 = y + h - 1;

	Region text_r;
	text_r.x1 = bounds.x1 + margin + indent;
	text_r.x2 = bounds.x2 - margin;
	text_r.width = text_r.x2 - text_r.x1 + 1;
	text_r.y1 = bounds.y1 + margin;
	text_r.y2 = bounds.y2 - margin;
	text_r.height = text_r.y2 - text_r.y1 + 1;

	//gc.setFill( Color::WHITE );
	//gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );

	//gc.setForeground( Color::LIGHT_GREY );
	//gc.drawRectangle( text_r.x1, text_r.y1, text_r.width, text_r.height );

	
	int offset = text_r.y1 + this->lineSpacing;
	Iterator* it = this->lines->iterator();
	TextExtents* extents;
	while ( it->hasNext() )
	{
		const String& line = dynamic_cast<const String&>( it->next() );
		extents = gc.measureTextExtents( line );
		offset += extents->ascent;
		
		gc.setForeground( Color::BLACK );
		gc.drawString( line, text_r.x1, offset );
		offset += (extents->descent + this->lineSpacing);
		delete extents;
	}
	delete it;


/*
	gc.setForeground( Color::DARK_GREY );
	TextExtents* extents = gc.measureTextExtents( *this->TextArea );

	unsigned int sx;
	unsigned int sy;
	
	switch ( hints & Textual::ALIGN )
	{
	case Textual::LEFT:
		sx = x + margin + indent;
		break;
	case Textual::CENTERED:
		sx = x + margin + ((width/2) - (extents->width/2));
		break;
	};
	
	sy = y + (height/2) + (extents->ascent/2);
	gc.drawString( *this->TextArea, sx, sy );
	delete extents;
*/

	return false;
}

const String&
TextArea::getText() const
{
	return *this->text;
}

const String&
TextArea::getData() const
{
	return *this->text;
}

void
TextArea::breakLines( const GraphicsContext& gc, unsigned int maxLineWidth )
{
	int length = this->text->getLength();
	int start = 0;
	int end = length - 1;

	String* trial;
	unsigned int width;

	this->lines->removeAllElements();

	while ( start < length )
	{
		int i;

		for ( i = end; i >= start; i-- )
		{
			trial = this->text->substring( start, i );
			width = gc.measureTextWidth( *trial );

			if ( width < maxLineWidth )
			{
				this->lines->add( trial );
				start = i + 1;
			} else if ( i == start ) {
				delete trial;
				start = length;
				break;
			} else {
				delete trial;
			}
		}
	}
}


/*
void
TextArea::breakLines( const GraphicsContext& gc, unsigned int maxLineWidth )
{
	int length = this->text->getLength();
	int start = 0;
	int end = length - 1;

	String* trial;
	unsigned int width;
	char c;

	this->lines->removeAllElements();

	while ( start < length )
	{
	
	
	
		bool splitable = false;
		int i;
		for ( i = end; i >= start; i-- )
		{
			c = this->text->charAt( i );
			if ( (' ' == c) || ('-' == c) || ('_' == c) )
			{
				splitable = true;
				break;
			}
		}
		
		if ( splitable )
		{
			for ( i = end; i >= start; i-- )
			{
				c = this->text->charAt( i );
				if ( (' ' == c) || ('-' == c) || ('_' == c) || ( i == end ) )
				{
					if ( ' ' == c ) i--;
					trial = this->text->substring( start, i );
					width = gc.measureTextWidth( *trial );

					if ( width < maxLineWidth )
					{
						this->lines->add( trial );
						start = i + 1;
						if ( (start < length) && (' ' == this->text->charAt( start )) )
						{
							start++;
						}
						break;
					} else {
						delete trial;
					}
				}
				else if ( i == start )
				{
					start = length;
					break;
				}
			}
		} else {
			for ( i = end; i >= start; i-- )
			{
				trial = this->text->substring( start, i );
				width = gc.measureTextWidth( *trial );

				if ( width < maxLineWidth )
				{
					this->lines->add( trial );
					start = i + 1;
				} else {
					delete trial;
				}
			}
		}
	}
}
*/

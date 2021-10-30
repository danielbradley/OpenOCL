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
#include "openocl/ui/IconText.h"

#include <openocl/base/FormattedString.h>
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
#include <openocl/util/StringTokenizer.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int IconText::padding = 1;

IconText::IconText( const String& aString ) : Textual( aString )
{
	this->setTextLayoutHints( Textual::LEFT );
	this->setMargin( 2 );
	this->setIndent( 0 );
	this->lineSpacing = 2;
	this->text = new String();
	this->tokens = new Sequence();

	this->bounds  = new Region();
	this->outline = new Region();
}

IconText::IconText( const IconText& iconText ) : Textual( iconText.getName() )
{
	fprintf( stderr, "IconText::IconText( const IconText& ): shouldn't be cloning an IconText, aborting!\n" );
	abort();
}

IconText::~IconText()
{
	delete this->text;
	delete this->tokens;

	delete this->bounds;
	delete this->outline;
}

//-------------------------------------------------------------------------
//	public virtual methods (Container)
//-------------------------------------------------------------------------

void
IconText::doLayout( const GraphicsContext& gc )
{
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

	*this->bounds  = bounds;
	*this->outline = outline;	
}


//-------------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------------

void
IconText::setText( const String& text )
{
	StringTokenizer st( text );
	st.setBreakOnCaps( true );
	st.setDelimiters( " .-_" );

	delete this->text;
	this->text = text.toString();
	delete this->tokens;
	this->tokens = st.tokenize();
}

//-------------------------------------------------------------------------
//	public virtual constant methods (Component)
//-------------------------------------------------------------------------

Dimensions
IconText::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;

	int available_width = width - (2*this->getMargin());

	//
	//	Iterate through tokens and determine desired height given width
	//

	dim.width  = width;
	dim.height = this->lineSpacing;

	StringBuffer* test  = new StringBuffer();
	StringBuffer* write = new StringBuffer();

	//fprintf( stderr, "IconText::getPreferredDimensions: %s\n", this->text->getChars() );

	Iterator* it = this->tokens->iterator();
	while ( it->hasNext() )
	{
		//	While there are more tokens append to teststring and check if larger
		//	than width write writablestring.

		const String& str = dynamic_cast<const String&>( it->next() );
		//fprintf( stderr, "IconText::getPreferredDimensions: it->next() : %s\n", str.getChars() );
		test->append( str );
		
		int text_width = gc.measureTextWidth( test->asString() );
		if ( text_width <= available_width )
		{
			write->append( str );
		}
		else
		{
			const String& line = write->asString();
			TextExtents* extents = gc.measureTextExtents( line );
			{
				//int x_offset = outline.x1 + ((outline.width/2) - (extents->width/2));
				dim.height += extents->ascent;
				dim.height += extents->descent;
				dim.height += this->lineSpacing;

				delete write;
				write = new StringBuffer();
				write->append( str );
				
				delete test;
				test = new StringBuffer();
				test->append( str );
				
			}
			delete extents;
		}
	}
	delete it;

	//	Add the last line
	{
		const String& line = write->asString();
		TextExtents* extents = gc.measureTextExtents( line );
		{
			dim.height += extents->ascent;
			dim.height += extents->descent;
			dim.height += this->lineSpacing;
		}
		delete extents;
	}
	
	delete test;
	delete write;


	/*
	{
		dim.width = width;
		dim.height += this->lineSpacing + (2*this->getMargin());
	
		unsigned int h = 0;
	
		Iterator* it = this->tokens->iterator();
		while ( it->hasNext() )
		{
			const String& token = dynamic_cast<const String&>( it->next() );
			h = gc.measureTextHeight( token );
			if ( it->hasNext() )
			{
				const String& token = dynamic_cast<const String&>( it->next() );
				h = Math::max( h, gc.measureTextHeight( token ) );
			}
			dim.height += (h + this->lineSpacing);
		}
		delete it;
	
		dim.height = Math::min( dim.height, height );
	}
	*/


	return dim;
}

bool
IconText::draw( GraphicsContext& gc ) const
{
	Region bounds = *this->bounds;
	Region outline = *this->outline;

	int offset = outline.y1 + this->lineSpacing;

	StringBuffer* test  = new StringBuffer();
	StringBuffer* write = new StringBuffer();

	Iterator* it = this->tokens->iterator();
	while ( it->hasNext() )
	{
		//	While there are more tokens append to teststring and check if larger
		//	than width write writablestring.

		const String& str = dynamic_cast<const String&>( it->next() );
		test->append( str );
		
		int text_width = gc.measureTextWidth( test->asString() );
		if (  text_width < outline.width )
		{
			write->append( str );
		}
		else
		{
			const String& line = write->asString();
			//fprintf( stderr, "IconText::draw(): %s\n", line.getChars() );
			TextExtents* extents = gc.measureTextExtents( line );
			{
				int x_offset = outline.x1 + ((outline.width/2) - (extents->width/2));
				offset += extents->ascent;
				gc.setForeground( Color::BLACK );
				gc.drawString( line, x_offset, offset );
				delete write;
				write = new StringBuffer();
				write->append( str );
				
				delete test;
				test = new StringBuffer();
				test->append( str );
				
				offset += extents->descent;
				offset += this->lineSpacing;
			}
			delete extents;
		}
	}
	delete it;

	//	Write the last line
	{
		const String& line = write->asString();
		//fprintf( stderr, "IconText::draw(): %s\n", line.getChars() );
		TextExtents* extents = gc.measureTextExtents( line );
		{
			int x_offset = outline.x1 + ((outline.width/2) - (extents->width/2));
			offset += extents->ascent;
			gc.setForeground( Color::BLACK );
			gc.drawString( line, x_offset, offset );
		}
		delete extents;
	}
	
	delete test;
	delete write;

	return false;
}

const String&
IconText::getData() const
{
	return *this->text;
}

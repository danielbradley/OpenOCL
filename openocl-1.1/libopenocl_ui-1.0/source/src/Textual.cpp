/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/Textual.h"

#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;

double Textual::mm2pixel = 1.0;
double Textual::pixel2mm = 1.0;

Textual::Textual() : Component( "textual" )
{
	this->setMargin( DEFAULT_MARGIN );
	this->setPadding( DEFAULT_INDENT );

	this->text = new Region();

	this->indent   = DEFAULT_INDENT;
	this->spacing  = DEFAULT_SPACING;
	this->tabWidth = DEFAULT_TABWIDTH;

	this->textHints = LayoutManager::LEFT | LayoutManager::MIDDLE;
}

Textual::Textual( const String& aString ) : Component( aString )
{
	this->setMargin( DEFAULT_MARGIN );
	this->setPadding( DEFAULT_INDENT );

	this->text = new Region();

	this->indent      = DEFAULT_INDENT;
	this->rightIndent = DEFAULT_INDENT;
	this->spacing     = DEFAULT_SPACING;
	this->tabWidth    = DEFAULT_TABWIDTH;

	this->textHints = LayoutManager::LEFT | LayoutManager::MIDDLE;
}

Textual::~Textual()
{
	delete this->text;
}

void
Textual::doLayout( const GraphicsContext& gc )
{
	this->Component::doLayout( gc );

	Region content = this->getContentRegion();

	int spacing = this->spacing;
	//  int cy      = content.y1 + (content.height/2);

	Dimensions d = this->getPreferredTextDimensions( gc );
	TextExtents* extents = gc.measureTextExtents( this->getData() );

	Region text;
	int cx;
	switch ( this->textHints )
	{
	case Textual::CENTERED:
		cx = content.x1 + content.width/2;
		text.width  = extents->width;
		text.x1 = cx - text.width/2;
		text.x2 = text.x1 + text.width - 1;
		break;
	case Textual::RIGHT:
		text.x2    = content.x2 - this->indent;
		text.width = extents->width;
		text.x1    = text.x2 - text.width + 1;
		break;
	case Textual::LEFT:
	default:
		text.x1     = content.x1 + this->indent;
		text.width  = extents->width;
		text.x2     = text.x1 + text.width - 1;
	}
	
	
		//	If you wanted the text region to be strictly the
		//	area of the text then uncomment the two lines below
		//	However, this has unfortunate interactions in components
		//	such as TextFields that change contents of their text
		//	between layouts.
		//
		//text.width  = Math::min( (text.x2 - text.x1 + 1), d.width );
		//text.x2     = text.x1 + text.width - 1;

	text.y2 = content.y2 - spacing - extents->descent;
	text.height = extents->ascent;
	text.y1 = text.y2 - text.height + 1;

	delete extents;

	*this->text    = text;
}

void
Textual::setIndent( int value )
{
	this->indent = value;
}

void
Textual::setIndentMM( int value )
{
	this->indent = (int) (value * mm2pixel);
}

void
Textual::setRightIndent( int value )
{
	this->rightIndent = value;
}

void
Textual::setRightIndentMM( int value )
{
	this->rightIndent = (int) (value * mm2pixel);
}

void
Textual::setMarginMM( int value )
{
	this->setMargin( (int) (value * mm2pixel) );
}

void
Textual::setSpacing( unsigned int value )
{
	this->spacing = value;
}

void
Textual::setTabWidth( unsigned int value )
{
	this->tabWidth = value;
}

void
Textual::setTabWidthMM( unsigned int value )
{
	this->tabWidth = (int) (value * mm2pixel);
}

void
Textual::setTextLayoutHints( flags layoutHints )
{
	this->textHints = layoutHints;
}

Dimensions
Textual::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	int total_space = this->getContentInset() * 2;
	
	gc.setDefaultFont();
	Dimensions text = this->getPreferredTextDimensions( gc );
	Dimensions preferred;
	preferred.width  = text.width  + total_space;
	preferred.height = text.height + total_space;

#ifdef DEBUG_OPENOCL_UI_TEXTUAL
	fprintf( stderr, "Textual::getPreferredDimensions():" );

	fprintf( stderr, " width:  %u = %u + %u + %u + %u + %u", preferred.width, text.width, padding*2, margin*2, left, right );
	fprintf( stderr, " height: %u = %u + %u + %u + %u", preferred.height, text.height, padding*2, margin*2, spacing*2 );

	fprintf( stderr, "\n" );
#endif

	//preferred.height += 5;
	//preferred.height = Math::max( preferred.height, 20 );

	return preferred;
}

Dimensions
Textual::getPreferredTextDimensions( const GraphicsContext& gc ) const
{
	Dimensions d;
	{
		TextExtents* extents = gc.measureTextExtents( this->getData() );
		d.width = extents->width;
		d.height = extents->ascent + extents->descent * 2 + this->spacing;
		delete extents;
	}
	return d;
}

bool
Textual::draw( GraphicsContext& gc ) const
{
	Region bounds  = this->getBounds();
	gc.setForeground( Color::RED );
	gc.drawRectangleRegion( bounds );

	Region edge    = this->getEdgeRegion();
	gc.setForeground( Color::GREEN );
	gc.drawRectangleRegion( edge );

	Region margin  = this->getMarginRegion();
	gc.setForeground( Color::BLUE );
	gc.drawRectangleRegion( margin );

	Region text    = this->getTextRegion();
	gc.setForeground( Color::BLACK );
	gc.drawRectangleRegion( text );

	return false;
}

Region
Textual::getTextRegion() const
{
	return *this->text;
}

int
Textual::getIndent() const
{
	return this->indent;
}

int
Textual::getIndentMM() const
{
	return (int) (this->indent * pixel2mm);
}

int
Textual::getRightIndent() const
{
	return this->rightIndent;
}

int
Textual::getRightIndentMM() const
{
	return (int) (this->rightIndent * pixel2mm);
}

int
Textual::getMarginMM() const
{
	return (int) (this->getMargin() * pixel2mm);
}

unsigned int
Textual::getTabWidth() const
{
	return this->tabWidth;
}

unsigned int
Textual::getTabWidthMM() const
{
	return (int) (this->tabWidth * pixel2mm);
}

flags
Textual::getTextLayoutHints() const
{
	return this->textHints;
}

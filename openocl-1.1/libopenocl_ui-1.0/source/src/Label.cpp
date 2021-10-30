/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Label.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

Label::Label( const String& aString ) : Textual( aString )
{
	this->setEdge( 10 );
	this->setMargin( 2 );
	this->setBorder( 0 );
	this->setPadding( 2 );

	this->setTextLayoutHints( Textual::MIDDLE | Textual::LEFT );
	this->setSpacing( 0 );
	this->setIndent( 10 );
	this->setRightIndent( 10 );

	this->label = aString.toString();

	this->preferredWidth = 0;
}

Label::~Label()
{
	delete this->label;
}

/*
Dimensions
Label::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	unsigned int margin = this->getMargin();
	unsigned int indent = this->getIndent();
	
	TextExtents* extents = gc.measureTextExtents( *this->label );
	dim.width  = extents->width  + padding*2 + margin*2 + indent;
	dim.height = extents->ascent + padding*2 + margin*2;	
	delete extents;
	
	return dim;
}
*/

bool
Label::draw( GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	Region text   = this->getTextRegion();

	flags hints = this->getTextLayoutHints();

	//gc.setForeground( Color::TOOLBAR );
	//gc.setFill( Color::TOOLBAR );
	//gc.setBackground( Color::TOOLBAR );
	//gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );

	gc.setForeground( Color::DARK_GREY );

	int sx = 0;

	switch ( hints & Textual::ALIGN )
	{
	case Textual::LEFT:
		sx = text.x1;
		break;
	case Textual::CENTERED:
		sx = text.x1 + (text.width/2);
		break;
	};
	
	gc.drawString( *this->label, text.x1, text.y2 );

#ifdef DEBUG_OPENOCL_UI_LABEL
	//gc.setFill( Color::RED );
	//gc.drawFilledRectangleRegion( bounds );
	this->Textual::draw( gc );
#endif

#ifdef DEBUG_OPENOCL_UI_TEXTUAL
	this->Textual::draw( gc );
#endif

	return false;
}

void
Label::setText( const String& aLabel )
{
	delete this->label;
	this->label = new String( aLabel );
}

void
Label::setPreferredWidth( unsigned int aWidth )
{
	this->preferredWidth = aWidth;
}

const String&
Label::getData() const
{
	return *this->label;
}

Dimensions
Label::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
#ifdef DEBUG_OPENOCL_UI_TEXTFIELD_GETPREFERREDDIMENSIONS
	fprintf( stderr, "TextField::getPreferredDimensions( gc, %i, %i )\n", width, height );
#endif

	Dimensions d = this->Textual::getPreferredDimensions( gc, width, height );
	
	if ( 0 < this->preferredWidth )
	{
		d.width = this->preferredWidth;
	}
	else if ( (d.width < width) && (50 < width) )
	{
		d.width = Math::max( d.width, 50 );
	}

	d.width  = Math::min( width, d.width );
	d.height = Math::min( height, d.height );

	return d;
}


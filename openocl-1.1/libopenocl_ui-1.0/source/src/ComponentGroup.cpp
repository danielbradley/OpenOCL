/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ComponentGroup.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"

#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;

ComponentGroup::ComponentGroup( const Color& aColor ) : Container()
{
	this->setMargin( 10 );
	this->setBorder( 2 );
	this->setPadding( 10 );
}

//ComponentGroup::ComponentGroup( const Color& aColor ) : OpaqueContainer( aColor )
//{
//	this->setPadding( 20 );
//}

/*
Dimensions
ComponentGroup::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	int total_padding = this->getPadding() * 2;
	unsigned int adjusted_width  = width  - total_padding;
	unsigned int adjusted_height = height - total_padding;
	
	if ( adjusted_width  > width )  adjusted_width  = 0; 
	if ( adjusted_height > height ) adjusted_height = 0; 
	
	Dimensions d = this->Container::getPreferredDimensions( gc, adjusted_width, adjusted_height );
	d.width  += total_padding;
	d.height += total_padding;

	return d;
}
*/

bool
ComponentGroup::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	bool changed = this->hasChanged();
	//bool redraw_all = this->isRedrawAll();

	//bool draw_again = this->OpaqueContainer::draw( gc );
	if ( changed )
	{
		draw_again = this->Container::draw( gc );
	}

	if ( changed )//redraw_all && changed )
	{
		Region bounds  = this->getBounds();
		Region border1 = this->getBorderRegion();
		Region border2 = border1.shrunkBy( 1 );

		if ( false )
		{
			Region box = this->getBox();
			gc.setForeground( Color::RED );
			gc.drawRectangleRegion( box );
			fprintf( stderr, "ComponentGroup::draw: %u %u\n", box.width, box.height );
		}

		this->drawInward3DBox( gc, border1, Color::SHINE2, Color::SHADE1 );
		this->drawOutward3DBox( gc, border2, Color::SHINE2, Color::SHADE1 );

		const String& name = this->getName();
		unsigned int name_width = gc.measureTextWidth( name );
		if ( 0 < name_width )
		{
			//	1)  Create text region
			Region text = bounds;
			text.height = 15;
			text.y2 = text.y1 + text.height - 1;
			text.translate( 30, 6 );

			//	2) Make text region the width of text + 5 margin on each side (i.e +10)
			//     then draw filled rectangle.
			text.width = name_width + 10;
			text.x2 = text.x1 + text.width - 1;
			gc.setFill( gc.getBackground() );
			gc.drawFilledRectangleRegion( text );
			
			//	3) Tranlate text region right.
			text.translate( 5, 0 );
			gc.setForeground( Color::WHITE );
			gc.drawBoundedString( name, text );
			
			text.translate( 1, 1 );
			gc.setForeground( Color::INACTIVE_TEXT );
			gc.drawBoundedString( name, text );
		}
		this->setChanged( false );
		this->setRedrawAll( false );
	}

	return draw_again;
}

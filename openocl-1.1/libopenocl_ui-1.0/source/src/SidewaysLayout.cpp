/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SidewaysLayout.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

SidewaysLayout::SidewaysLayout()
{
	this->layoutHints = LayoutManager::EASTWARD;
}

SidewaysLayout::SidewaysLayout( flags hints )
{
	this->layoutHints = hints;
}

SidewaysLayout::~SidewaysLayout()
{}

Dimensions
SidewaysLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	const Sequence& components = aContainer.getComponents();
	Dimensions preferred;

	int max_row_w = 0;
	int max_row_h = 0;

	int x_offset = 0;
	int y_offset = 0;
	
	int rem_w = width;
	int rem_h = height;
	
	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		Dimensions dim = com.getPreferredDimensions( gc, rem_w, rem_h );

		if ( ((int)dim.width) > rem_w )
		{
			//	If the component doesn't fit in this row go to next row:
			//	1) record the width of this row in "max_row_w" if the biggest row so far.
			//	2) increase the y_offset by the height of the largest component in this
			//         row (max_row_h) then zero max_row_h.
			//	3) recalculate the remaining height left in the Container (height - y_offset)
			//	4) intialize x_offset to 0.
			//	5) initialize remaining width (rem_w)
			//	6) recheck preferred dimensions of componenet in new row.

			max_row_w = Math::max( max_row_w, x_offset );    // (1)
			y_offset += max_row_h; max_row_h = 0;            // (2)
			rem_h     = height - y_offset;                   // (3)
			x_offset  = 0;                                   // (4)
			rem_w     = width;                               // (5)
			dim       = com.getPreferredDimensions( gc, rem_w, rem_h );
		}

		//	If here, there is either enough room for the component or we have moved
		//	to a new row and there still isn't enough room. Either way the component will
		//	now have too squeeze itself into the room we provide it.

		x_offset += dim.width;
		rem_w    -= dim.width;
		max_row_h = Math::max( max_row_h, dim.height );
	}
	delete it;

	//	Update max_row_w(idth) to take into account last row
	//	Increment y_offset by height of last row;
	//	then return preferred dimensions.
	
	max_row_w        = Math::max( max_row_w, x_offset );
	y_offset        += max_row_h;
	preferred.width  = Math::min( width, max_row_w );
	preferred.height = Math::min( height, y_offset );		
	
	return preferred;
}

void
SidewaysLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	if ( LayoutManager::EASTWARD == (LayoutManager::EASTWARD & this->layoutHints) )
	{
		this->doEastwardLayout( aContainer, gc );
	} else {
		this->doWestwardLayout( aContainer, gc );
	}
}

void
SidewaysLayout::doEastwardLayout( Container& aContainer, const GraphicsContext& gc ) const
{
#ifdef DEBUG_OPENOCL_UI_SIDEWAYSLAYOUT
	fprintf( stderr, "SidewaysLayout::doEastwardLayout()\n" );
#endif
	Region bounds = aContainer.getLayoutRegion();

	const Sequence& components = aContainer.getComponents();

	int max_row_w = 0;
	int max_row_h = 0;

	int x_offset = bounds.x1;
	int y_offset = bounds.y1;
	
	int rem_w = bounds.width;
	int rem_h = bounds.height;

	unsigned int row_start = 0;
	unsigned int row_end = 0;
	
	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		Dimensions dim = com.getPreferredDimensions( gc, rem_w, rem_h );

		if ( ((int)dim.width) > rem_w )
		{
			//	We now go back and change the heights of each component
			//	in this row to be equal.
			
			for ( unsigned int i=row_start; i < row_end; i++ )
			{
				Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( i ) ) );
				Region com_bounds = com.getBounds();
				com.setBounds( com_bounds.x1, com_bounds.y1, com_bounds.width, max_row_h );
#ifdef DEBUG_OPENOCL_UI_SIDEWAYSLAYOUT
				fprintf( stderr, "SidewaysLayout::doEastwardLayout: %3i - ", i );
				com_bounds = com.getBounds();
				com_bounds.print( stderr );
#endif
			}
			row_start = row_end;
			row_end   = row_start;

			//	If the component doesn't fit in this row go to next row:
			//	1) record the width of this row in "max_row_w" if the biggest row so far.
			//	2) increase the y_offset by the height of the largest component in this
			//         row (max_row_h) then zero max_row_h.
			//	3) recalculate the remaining height left in the Container (height - y_offset)
			//	4) intialize x_offset to bounds.x1.
			//	5) initialize remaining width (rem_w)
			//	6) recheck preferred dimensions of componenet in new row.

			max_row_w = Math::max( max_row_w, x_offset );    // (1)
			y_offset += max_row_h; max_row_h = 0;            // (2)
			rem_h     = bounds.height - y_offset;            // (3)
			x_offset  = bounds.x1;                           // (4)
			rem_w     = bounds.width;                        // (5)
			dim       = com.getPreferredDimensions( gc, rem_w, rem_h );
		}

		//	If here, there is either enough room for the component or we have moved
		//	to a new row and there still isn't enough room. Either way the component will
		//	now have too squeeze itself into the room we provide it.

		//com.setBounds( x_offset, y_offset, Math::min(  rem_w, dim.width ), Math::min( rem_h, dim.height ) );
		com.setBounds( x_offset, y_offset, dim.width, dim.height );
		com.doLayout( gc );

		x_offset += dim.width;
		rem_w    -= dim.width;
		max_row_h = Math::max( max_row_h, dim.height );
		
		row_end++;
	}
	delete it;

	for ( unsigned int i=row_start; i < row_end; i++ )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( i ) ) );
		Region com_bounds = com.getBounds();
		com.setBounds( com_bounds.x1, com_bounds.y1, com_bounds.width, max_row_h );
		com.doLayout( gc );

#ifdef DEBUG_OPENOCL_UI_SIDEWAYSLAYOUT
		fprintf( stderr, "SidewaysLayout::doEastwardLayout: %3i - ", i );
		com_bounds = com.getBounds();
		com_bounds.print( stderr );
#endif
	}
}

void
SidewaysLayout::doWestwardLayout( Container& aContainer, const GraphicsContext& gc ) const
{
#ifdef DEBUG_OPENOCL_UI_SIDEWAYSLAYOUT
	fprintf( stderr, "SidewaysLayout::doWestwardLayout()\n" );
#endif

	Region bounds = aContainer.getLayoutRegion();

	Sequence& components = aContainer.getComponents();

	int max_row_w = 0;
	int max_row_h = 0;

	int x_offset = bounds.x2;
	int y_offset = bounds.y1;
	
	int rem_w = bounds.width;
	int rem_h = bounds.height;
	
	int last = components.getLength() - 1;
		
	for ( int i=last; i >= 0; i-- )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i ) );
		Dimensions dim = com.getPreferredDimensions( gc, rem_w, rem_h );

		if ( ((int)dim.width) > rem_w )
		{
			//	If the component doesn't fit in this row go to next row:
			//	1) record the width of this row in "max_row_w" if the biggest row so far.
			//	2) increase the y_offset by the height of the largest component in this
			//         row (max_row_h) then zero max_row_h.
			//	3) recalculate the remaining height left in the Container (height - y_offset)
			//	4) intialize x_offset to bounds.x1.
			//	5) initialize remaining width (rem_w)
			//	6) recheck preferred dimensions of componenet in new row.

			max_row_w = Math::max( max_row_w, x_offset );    // (1)
			y_offset += max_row_h; max_row_h = 0;            // (2)
			rem_h     = bounds.height - y_offset;            // (3)
			x_offset  = bounds.x2;                           // (4)
			rem_w     = bounds.width;                        // (5)
			dim       = com.getPreferredDimensions( gc, rem_w, rem_h );
		}

		//	If here, there is either enough room for the component or we have moved
		//	to a new row and there still isn't enough room. Either way the component will
		//	now have too squeeze itself into the room we provide it.

		//com.setBounds( x_offset, y_offset, Math::min(  rem_w, dim.width ), Math::min( rem_h, dim.height ) );
		int x = x_offset - dim.width + 1;
		
		com.setBounds( x, y_offset, dim.width, dim.height );
		com.doLayout( gc );

		x_offset -= dim.width;
		rem_w    -= dim.width;
		max_row_h = Math::max( max_row_h, dim.height );
	}
}


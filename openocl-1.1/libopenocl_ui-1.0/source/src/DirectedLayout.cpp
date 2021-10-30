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
#include "openocl/ui/DirectedLayout.h"

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

DirectedLayout::DirectedLayout( flags hints )
{
	this->layoutHints = hints;
}

DirectedLayout::~DirectedLayout()
{}

Dimensions
DirectedLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	const Sequence& components = aContainer.getComponents();
	Dimensions preferred;

	int max_w = 0;
	int max_h = 0;
	
	int sum_widths = 0;
	int sum_heights = 0;

	int rem_w = width;
	int rem_h = height;

	bool horizontal = (LayoutManager::WESTWARD == (this->layoutHints & LayoutManager::WESTWARD));
	horizontal     |= (LayoutManager::EASTWARD == (this->layoutHints & LayoutManager::EASTWARD));

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		Dimensions dim = com.getPreferredDimensions( gc, rem_w, rem_h );

		max_w = Math::max( max_w, dim.width );
		max_h = Math::max( max_h, dim.height );

		sum_widths += dim.width;
		sum_heights += dim.height;
		
		if ( horizontal )
		{
			rem_w -= dim.width;
		} else {
			rem_h -= dim.height;
		}
	}
	delete it;

	if ( horizontal )
	{
		preferred.width = sum_widths;
		preferred.height = max_h;
	}
	else
	{
		preferred.width = max_w;
		preferred.height = sum_heights;
	}
	
	return preferred;
}

void
DirectedLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	if ( LayoutManager::EASTWARD == (LayoutManager::EASTWARD & this->layoutHints) )
	{
		this->doEastwardLayout( aContainer, gc );
	}
	else if ( LayoutManager::WESTWARD == (LayoutManager::WESTWARD & this->layoutHints) )
	{
		this->doWestwardLayout( aContainer, gc );
	}
	else if ( LayoutManager::SOUTHWARD == (LayoutManager::SOUTHWARD & this->layoutHints) )
	{
		this->doSouthwardLayout( aContainer, gc );
	}
	else if ( LayoutManager::NORTHWARD == (LayoutManager::NORTHWARD & this->layoutHints) )
	{
		this->doNorthwardLayout( aContainer, gc );
	}
}

void
DirectedLayout::doEastwardLayout( Container& container, const GraphicsContext& gc ) const
{
	Region edge = container.getEdgeRegion();
	
	Sequence& components = container.getComponents();
	int max    = components.getLength();
	
	int x_offset = edge.x1;
	int rem_w  = edge.width;
	int height = edge.height;

	for ( int i=0; i < (max-1); i++ )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i ) );
		Dimensions preferred = com.getPreferredDimensions( gc, rem_w, height );
		
		preferred.width = Math::min( preferred.width, rem_w );
		
		com.setBounds( x_offset, edge.y1, preferred.width, height );
		com.doLayout( gc );
		rem_w    -= preferred.width;
		x_offset += preferred.width;
	}
	
	if ( 0 < max )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( max - 1 ) );
		com.setBounds( x_offset, edge.y1, rem_w, height );
		com.doLayout( gc );
	}
}

void
DirectedLayout::doWestwardLayout( Container& container, const GraphicsContext& gc ) const
{
	Region edge = container.getContentRegion();
	
	Sequence& components = container.getComponents();
	int max    = components.getLength();
	
	int x2_offset = edge.x2;
	int rem_w  = edge.width;
	int height = edge.height;

	for ( int i=max; i > 1; i-- )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i-1 ) );
		Dimensions preferred = com.getPreferredDimensions( gc, rem_w, height );
		com.setBounds( x2_offset - preferred.width, edge.y1, preferred.width, height );
		com.doLayout( gc );
		rem_w    -= preferred.width;
		x2_offset -= preferred.width;
	}

	//	Last component is stretched to fill area.
	Component& com = dynamic_cast<Component&>( components.elementAt( 0 ) );
	com.setBounds( edge.x1, edge.y1, rem_w, height );
	com.doLayout( gc );
}

void
DirectedLayout::doSouthwardLayout( Container& container, const GraphicsContext& gc ) const
{
	Region edge = container.getEdgeRegion();
	
	Sequence& components = container.getComponents();
	int max    = components.getLength();
	
	int y_offset = edge.y1;
	int width = edge.width;
	int rem_h = edge.height;
	
	for ( int i=0; i < (max - 1); i++ )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i ) );
		Dimensions preferred = com.getPreferredDimensions( gc, width, rem_h );

		preferred.height = Math::min( preferred.height, rem_h );

		com.setBounds( edge.x1, y_offset, width, preferred.height );
		com.doLayout( gc );
		rem_h    -= preferred.height;
		y_offset += preferred.height;
	}

	if ( 0 < max )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( max - 1 ) );
		com.setBounds( edge.x1, y_offset, width, rem_h );
		com.doLayout( gc );
	}
}

void
DirectedLayout::doNorthwardLayout( Container& container, const GraphicsContext& gc ) const
{
	Region edge = container.getEdgeRegion();
	
	Sequence& components = container.getComponents();
	int max    = components.getLength();
	
	int y2_offset = edge.y2;
	int width     = edge.width;
	int rem_h     = edge.height;
	
	for ( int i=max; i > 1; i-- )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i - 1 ) );
		Dimensions preferred = com.getPreferredDimensions( gc, width, rem_h );
		com.setBounds( edge.x1, y2_offset - preferred.height, width, preferred.height );
		com.doLayout( gc );
		rem_h     -= preferred.height;
		y2_offset -= preferred.height;
	}
	Component& com = dynamic_cast<Component&>( components.elementAt( 0 ) );
	com.setBounds( edge.x1, edge.y1, width, rem_h );
	com.doLayout( gc );
}

/*
void
DirectedLayout::doNorthwardLayout( Container& aContainer, const GraphicsContext& gc ) const
{
#ifdef DEBUG_OPENOCL_UI_DirectedLayout
	fprintf( stderr, "DirectedLayout::doWestwardLayout()\n" );
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
*/

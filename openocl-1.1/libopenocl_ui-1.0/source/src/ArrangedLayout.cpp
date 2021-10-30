/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ArrangedLayout.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include "openocl/ui/Component.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

ArrangedLayout::ArrangedLayout()
{
}

ArrangedLayout::~ArrangedLayout()
{}

void
ArrangedLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	unsigned int x = aContainer.getX();
	unsigned int y = aContainer.getY();
	unsigned int container_width = aContainer.getWidth();

	Sequence& components = aContainer.getComponents();

	unsigned int max_width  = Math::max( 1, LayoutManager::maxWidth( components, gc ) );
	unsigned int max_height = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		max_height = Math::max( max_height, com.getPreferredDimensions( gc, max_width, 0xFFFF ).height );
	}
	delete it;
	aContainer.setLineHeight( max_height/4 );

	if ( max_width )
	{
		unsigned int per_row = container_width / max_width;

		unsigned int col = 0;
		unsigned int offset = y;

		Iterator* it = aContainer.getComponents().iterator();
		while( it->hasNext() )
		{
			Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
			com.setBounds( x + (col * max_width), offset, max_width, max_height );
			com.doLayout( gc );
			col++;
		
			if ( col == per_row )
			{
				col = 0;
				offset += max_height;
			}
		}
		delete it;
	}
}

Dimensions
ArrangedLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	const Sequence& components = aContainer.getComponents();

	unsigned int item_width  = Math::max( 1, LayoutManager::maxWidth( components, gc ) );
	unsigned int item_height = LayoutManager::maxHeight( components, gc );
	unsigned int items = components.getLength();
	unsigned int per_row = Math::max( 1, Math::min( items, width / item_width ) );
	unsigned int rows = items / per_row;

	if ( 0 != (items % per_row) )
	{
		rows++;
	}

	Dimensions dim;
	dim.width  = per_row * item_width;
	dim.height = item_height * rows;

	return dim;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphLayout.h"

#include "openocl/ui/Icon.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::util;
using namespace openocl::ui;

GraphLayout::GraphLayout()
{}

GraphLayout::~GraphLayout()
{}

void
GraphLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Region bounds = aContainer.getBounds();

	int mid_x = bounds.x1 + bounds.width/2;
	int mid_y = bounds.y1 + bounds.height/2;
	
	int x;
	int y;
	Dimensions   d;

	int ix;
	int iy;

	const Sequence& components = aContainer.getComponents();
	Iterator* it = components.iterator();

	while ( it->hasNext() )
	{
		Icon& icon = const_cast<Icon&>( dynamic_cast<const Icon&>( it->next() ) );
		if ( &icon )
		{
			x = icon.getPreferredX();
			y = icon.getPreferredY();
			d = icon.getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

			ix = (mid_x + x) - d.width/2;
			iy = (mid_y + y) - d.height/2;

			icon.setBounds( ix, iy, d.width, d.height );
			icon.doLayout( gc );
		}
	}
	delete it;
}

Dimensions
GraphLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions dim;

	int x;
	int y;
	int lo_x = 0;
	int hi_x = 0;
	int lo_y = 0;
	int hi_y = 0;
	
	Iterator* it = aContainer.getComponents().iterator();
	while ( it->hasNext() )
	{
		const Icon* icon = dynamic_cast<const Icon*>( &it->next() );
		if ( icon )
		{
			Dimensions d = icon->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
			x = icon->getPreferredX();
			y = icon->getPreferredY();

			lo_x = Math::min( lo_x, x - d.width/2 );
			hi_y = Math::max( hi_x, x + d.width/2 );
			
			lo_y = Math::min( lo_y, y - d.height/2 );
			hi_y = Math::max( hi_y, y + d.height/2 );
		}
	}
	delete it;

	dim.width  = hi_x - lo_x;
	dim.height = hi_y - lo_y;

	return dim;
}

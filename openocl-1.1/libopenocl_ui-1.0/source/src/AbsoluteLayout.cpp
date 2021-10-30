/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/AbsoluteLayout.h"

#include "openocl/ui/Icon.h"
#include "openocl/ui/Container.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::util;
using namespace openocl::ui;

AbsoluteLayout::AbsoluteLayout()
{}

AbsoluteLayout::~AbsoluteLayout()
{}

void
AbsoluteLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	unsigned int x;
	unsigned int y;
	Dimensions   d;

	const Sequence& components = aContainer.getComponents();
	Iterator* it = components.iterator();

	while ( it->hasNext() )
	{
		Icon& icon = const_cast<Icon&>( dynamic_cast<const Icon&>( it->next() ) );
		if ( &icon )
		{
			x = icon.getPreferredX();
			y = icon.getPreferredY();
			//d.set( icon.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ) );
			d = icon.getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

			icon.setBounds( x, y, d.width, d.height );
			icon.doLayout( gc );
		}
	}
	delete it;
}

Dimensions
AbsoluteLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	int x;
	int y;
	
	Iterator* it = aContainer.getComponents().iterator();
	while ( it->hasNext() )
	{
		const Icon* icon = dynamic_cast<const Icon*>( &it->next() );
		if ( icon )
		{
			Dimensions icon_dim = icon->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
			x = icon->getPreferredX() + icon_dim.width;
			y = icon->getPreferredY() + icon_dim.height;
		
			dim.width = Math::max( dim.width, x );
			dim.height = Math::max( dim.height, y );
		}
	}
	delete it;

	return dim;
}

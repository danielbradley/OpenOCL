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
#include "openocl/ui/LayoutManager.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::ui;
using namespace openocl::util;

LayoutManager::LayoutManager()
{}

LayoutManager::~LayoutManager()
{}

/*
unsigned int
LayoutManager::preferredLayoutWidth( const Container& aContainer, const GraphicsContext& gc ) const
{
	const Sequence& components = aContainer.getComponents();
	return this->preferredLayoutWidth( components, gc );
}

unsigned int
LayoutManager::preferredLayoutHeight( const Container& aContainer, const GraphicsContext& gc ) const
{
	const Sequence& components = aContainer.getComponents();
	return this->preferredLayoutHeight( components, gc );
}

Dimensions
LayoutManager::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	dim.width = this->preferredLayoutWidth( aContainer, gc );
	dim.height = this->preferredLayoutHeight( aContainer, gc );
	return dim;
}
*/

unsigned int
LayoutManager::maxWidth( const openocl::util::Sequence& components, const GraphicsContext& gc )
{
	unsigned int max = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		max = Math::max( max, com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ).width );
	}
	delete it;

	return max;
}

unsigned int
LayoutManager::maxHeight( const openocl::util::Sequence& components, const GraphicsContext& gc )
{
	unsigned int max = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		max = Math::max( max, com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ).height );
	}
	delete it;

	return max;
}

unsigned int
LayoutManager::sumOfWidths( const openocl::util::Sequence& components, const GraphicsContext& gc )
{
	unsigned int sum = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		sum += com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ).width;
	}
	delete it;

	return sum;
}

unsigned int
LayoutManager::sumOfHeights( const openocl::util::Sequence& components, const GraphicsContext& gc )
{
	unsigned int sum = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		sum += com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ).height;
	}
	delete it;

	return sum;
}

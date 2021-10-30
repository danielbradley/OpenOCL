/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */
#include "openocl/ui/SimpleLayout.h"

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

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

SimpleLayout::SimpleLayout()
{}

SimpleLayout::~SimpleLayout()
{}

/**
 *  This is called before the containers bounds are set.
unsigned int
SimpleLayout::preferredLayoutWidth( const Sequence& components,
                                     const GraphicsContext& gc ) const
{
	unsigned int preferred_width = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		preferred_width = Math::max( preferred_width, com.getPreferredWidth( gc ) );
	}
	delete it;

	return preferred_width;
}

unsigned int
SimpleLayout::preferredLayoutHeight( const Sequence& components,
                                      const GraphicsContext& gc ) const
{
	unsigned int preferred_height = 0;

	Iterator* it = components.iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		preferred_height += com.getPreferredHeight( gc );
	}
	delete it;

	return preferred_height;
}
 */

Dimensions
SimpleLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	const Sequence& components = aContainer.getComponents();
	Dimensions dim;
	dim.width  = LayoutManager::maxWidth( components, gc );		
	dim.height = LayoutManager::sumOfHeights( components, gc );
	return dim;
}

void
SimpleLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	unsigned int x = aContainer.getX();
	unsigned int y = aContainer.getY();
	unsigned int container_width = aContainer.getWidth();

	unsigned int offset = 0;
	unsigned int remaining_height = aContainer.getHeight();

	Iterator* it = aContainer.getComponents().iterator();
	while( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		Dimensions dim = com.getPreferredDimensions( gc, container_width, remaining_height );
		unsigned int height = Math::min( remaining_height, dim.height );

		com.setBounds( x, y + offset, container_width, height );
		com.doLayout( gc );
		offset += height;
		remaining_height -= height;
	}
	delete it;
}


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
#include "openocl/ui/StretchedLayout.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

StretchedLayout::StretchedLayout()
{
	this->hints = LayoutManager::SOUTHWARD;
}

StretchedLayout::StretchedLayout( flags layoutHints )
{
	this->hints = layoutHints;
}

StretchedLayout::~StretchedLayout()
{}

void
StretchedLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Region bounds = aContainer.getLayoutRegion();
	unsigned int x = bounds.x1;
	unsigned int y = bounds.y1;
	unsigned int container_width  = bounds.width;
	unsigned int container_height = bounds.height;

	unsigned int offset = 0;
	unsigned int override_height = 0;

	Dimensions dim;

	if ( LayoutManager::EXPANDED == (LayoutManager::EXPANDED & this->hints ))
	{
		unsigned int nr_of_elements = aContainer.getComponents().getLength();
		unsigned int largest = 0;
		unsigned int total = 0;

		Iterator* it = aContainer.getComponents().iterator();
		while( it->hasNext() )
		{
			Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
			dim = com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
			largest = Math::max( dim.height, largest );
			total += dim.height;
		}
		delete it;


		if ( ((container_height * 1.0) / (largest * 1.0) ) > (nr_of_elements * 1.0) )
		{
			 override_height = container_height / nr_of_elements;
		}
	}

	switch ( LayoutManager::NORTHWARD & this->hints )
	{
	case LayoutManager::NORTHWARD:
		{
			const Sequence& components = aContainer.getComponents();
			unsigned int nr_of_elements = components.getLength();
			offset = container_height;

			for ( unsigned int i=nr_of_elements; i > 0; i-- )
			{
				Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( i-1 ) ) );

				switch( override_height )
				{
				case 0:
					{
						dim = com.getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
						unsigned int height = Math::min( dim.height, offset );
						offset -= height; 
						com.setBounds( x, y + offset, container_width, height );
						com.doLayout( gc );
					}
					break;
				default:
					{
						offset -= override_height;
						com.setBounds( x, y + offset, container_width, override_height );
						com.doLayout( gc );
					}
				}
			}
		}
		break;
	default:
		{
			Iterator* it = aContainer.getComponents().iterator();
			while ( it->hasNext() )
			{
				Component& com = (Component&) it->next();
				dim = com.getPreferredDimensions( gc, container_width, container_height - offset );

				//	Width and height are used to store the height for each componenet.
				//    unsigned int width = Math::min( com.getPreferredWidth( gc ), containerWidth );
				unsigned int height;
				
				if ( override_height )
				{
					height = override_height;
				} else {
					height = dim.height;
				}

				com.setBounds( x, y + offset, container_width, height );
				com.doLayout( gc );
				offset += height;
			}
			delete it;
		}
	}
}

/**
 *  This is called before the containers bounds are set.
 */
 /*
unsigned int
StretchedLayout::preferredLayoutWidth( const Sequence& components,
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
StretchedLayout::preferredLayoutHeight( const Sequence& components,
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
StretchedLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions d;
	Dimensions component;

	Iterator* it = aContainer.getComponents().iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		component = com.getPreferredDimensions( gc, width, height );
		d.height += component.height;
		height   -= component.height;
	}
	delete it;
	d.width = width;

	return d;
}

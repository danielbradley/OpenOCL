/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ColumnLayout.h"

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
#include <openocl/util/Stack.h>

#include <cstdlib>

using namespace openocl::base;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

ColumnLayout::ColumnLayout()
{
	this->hints = ColumnLayout::EASTWARD;
}

ColumnLayout::ColumnLayout( flags layoutHints )
{
	this->hints = layoutHints;
}

ColumnLayout::~ColumnLayout()
{}

Dimensions
ColumnLayout::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	const Sequence& components = aContainer.getComponents();
	Dimensions dim;
	dim.width  = Math::min( width, LayoutManager::sumOfWidths( components, gc ) );
	dim.height = Math::min( height, LayoutManager::maxHeight( components, gc ) );
	return dim;
}

void
ColumnLayout::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Region bounds = aContainer.getBounds();
	unsigned int width = bounds.width;
	unsigned int height = bounds.height;

	unsigned int container_height = 0;
	unsigned int offset = 0;

	Sequence& components = aContainer.getComponents();
	unsigned int max = components.getLength();

	for ( unsigned int i=0; i < max; i++ )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i ) );
		container_height = Math::max( container_height, com.getPreferredDimensions( gc, width, height ).height );
	}
	container_height = Math::min( height , container_height );

	for ( unsigned int i=0; i < max; i++ )
	{
		Component& com = dynamic_cast<Component&>( components.elementAt( i ) );
		Dimensions d = com.getPreferredDimensions( gc, width, height );
		unsigned int com_width = Math::min( width, d.width );
		com.setBounds( offset, bounds.y1, com_width, container_height );
		com.doLayout( gc );
		offset += com_width;
		width -= com_width;
	}
}

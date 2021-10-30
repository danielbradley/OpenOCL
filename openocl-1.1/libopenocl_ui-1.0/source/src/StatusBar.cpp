/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/StatusBar.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SidewaysLayout.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

StatusBar::StatusBar()
: OpaqueContainer( Color::TOOLBAR )
{
	this->setPadding( 5 );
	this->setLayout( new SidewaysLayout() );
}

StatusBar::~StatusBar()
{
}

bool
StatusBar::draw( GraphicsContext& gc ) const
{
	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();
	bool draw_again = this->OpaqueContainer::draw( gc );

	if ( changed || redraw_all )
	{
		Region bounds = this->getBounds();
		Region moved = bounds;
		moved.y1++;
		moved.height = moved.y2 - moved.y1 + 1;
	
		Iterator* it = this->getComponents().iterator();
		while ( it->hasNext() )
		{
			const Component& com = dynamic_cast<const Component&>( it->next() );
			Region com_bounds = com.getBounds();
			if ( (com_bounds.width > 0) && (com_bounds.height > 0 ) )
			{
				Region inner = com_bounds.shrunkBy( 1 );
				this->drawInward3DBox( gc, com_bounds, Color::SHINE2, Color::SHADE1 );
				this->drawOutward3DBox( gc, inner, Color::SHINE2, Color::SHADE1 );
			}
		}
		delete it;

		this->drawOutward3DBox( gc, moved, Color::SHINE1, Color::SHADE2 );
		gc.setForeground( Color::SHADE2 );
		gc.drawLine( bounds.x1, bounds.y1, bounds.x2, bounds.y1 );
	}
	
	this->setChanged( false );
	this->setRedrawAll( false );
	
	return draw_again;
}

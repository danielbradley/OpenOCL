/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/IconGroup.h"

#include "openocl/ui/ArrangedLayout.h"
#include "openocl/ui/AbsoluteLayout.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/GraphLayout.h"
#include "openocl/ui/GridLayout.h"
#include "openocl/ui/Region.h"
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>

#include <cstdio>

using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

IconGroup::IconGroup()
{}

IconGroup::IconGroup( flags iconArrangement )
{
	//this->setEventMask( Event::MOUSE_EVENT | Event::MOTION_EVENT | Event::ACTION_EVENT );
	switch ( iconArrangement )
	{
	case ARRANGED:
		this->setLayout( new ArrangedLayout() );
		break;
	case FREEFORM:
		this->setLayout( new AbsoluteLayout() );
		break;
	case GRAPH:
		this->setLayout( new GraphLayout() );
	}
}

IconGroup::~IconGroup()
{}

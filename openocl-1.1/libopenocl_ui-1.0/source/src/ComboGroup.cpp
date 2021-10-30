/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/ComboGroup.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;

ComboGroup::ComboGroup() : OpaqueContainer( Color::CANVAS )
{}

ComboGroup::~ComboGroup()
{}

bool
ComboGroup::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	
	Region bounds;
	bounds.x1 = this->getX();
	bounds.y1 = this->getY();
	bounds.width = this->getWidth();
	bounds.height = this->getHeight();
	bounds.x2 = bounds.x1 + bounds.width - 1;
	bounds.y2 = bounds.y1 + bounds.height - 1;

	this->OpaqueContainer::draw( gc );

	gc.setForeground( Color::SHADOW_GREY );
	gc.drawRectangleRegion( bounds );
	//this->drawOutward3DBox( gc, bounds, Color::SHINY, Color::SHADE );

	return draw_again;
}

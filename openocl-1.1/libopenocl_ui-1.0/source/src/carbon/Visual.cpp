/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Visual.h"
#include "openocl/ui/carbon/CarbonVisual.h"

using namespace openocl::ui;

Visual*
Visual::getInstance()
{
	return new openocl::ui::carbon::CarbonVisual();
}

Visual::Visual()
{}

Visual::~Visual()
{}

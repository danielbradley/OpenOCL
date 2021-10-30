/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Visual.h"
#include "openocl/ui/win32/WinVisual.h"
#include <cstdio>

using namespace openocl::ui;

Visual*
Visual::getDefaultInstance()
{
	return new openocl::ui::win32::WinVisual();
}

Visual*
Visual::getInstance()
{
	fprintf( stderr, "Visual::getInstance() deprecated, use Visual::getDefaultInstance\n" );
	return new openocl::ui::win32::WinVisual();
}

Visual::Visual()
{}

Visual::~Visual()
{}

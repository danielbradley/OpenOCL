/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Blank.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Debug.h>
#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <stdio.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

Blank::Blank()
{}

Blank::~Blank()
{}

Dimensions
Blank::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	//return Dimensions( width, height );
	return Dimensions( 0x1, 0x1 );
}

bool
Blank::draw( GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	gc.setFill( gc.getBackground() );
	gc.drawFilledRectangleRegion( bounds ); 

#ifdef DEBUG_OPENOCL_UI_BLANK
	gc.setForeground( Color::RED );
	gc.drawRectangleRegion( bounds );
#endif

	return false;
}

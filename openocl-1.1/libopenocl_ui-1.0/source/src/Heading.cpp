/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Heading.h"
#include "openocl/ui/Region.h"

#include <openocl/base/String.h>
#include <openocl/imaging/Color.h>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;

Heading::Heading( const String& aString ) : Label( aString )
{}

Heading::~Heading()
{}

bool
Heading::draw( GraphicsContext& gc ) const
{
	bool draw_again = Label::draw( gc );
	
	Region box  = this->getEdgeRegion();
	Region text = this->getTextRegion();

	int cy = box.y1 + box.height/2;
	
	gc.setForeground( Color::SHADE1 );
	gc.drawLine( text.x2 + 10, cy, box.x2, cy );
	gc.setForeground( Color::SHINE1 );
	gc.drawLine( text.x2 + 10, cy+1, box.x2, cy+1 );

	return draw_again;
}

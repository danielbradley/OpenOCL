/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/Region.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;

OpaqueContainer::OpaqueContainer( const Color& backgroundColor )
: Container(), bgColor( backgroundColor )
{
	this->setEdge( 0 );
	this->setMargin( 0 );
	this->setBorder( 0 );
	this->setPadding( 0 );
}

OpaqueContainer::~OpaqueContainer()
{}

/*
bool
OpaqueContainer::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	gc.setBackground( this->bgColor );

	if ( this->hasChanged() )
	{	



		gc.setFill( this->bgColor );
		gc.setForeground( this->bgColor );
		gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );
		this->setChanged( false );
	}

	this->Container::draw( gc );
	
	return draw_again;
}
*/

bool
OpaqueContainer::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();

	Color original_background( gc.getBackground() );
	{
		gc.setBackground( this->bgColor );
		Region bounds = this->getBounds();

		gc.setClip( bounds.x1, bounds.y1, bounds.width, bounds.height );
		if ( redraw_all )
		{
#ifdef DEBUG_OPENOCL_UI_OPAQUECONTAINER_DRAW
			fprintf( stderr, "OpaqueContainer::draw: redraw all\n" );
#endif
			gc.setFill( this->bgColor );
			gc.drawFilledRectangleRegion( bounds );
		}

		if ( changed )
		{
#ifdef DEBUG_OPENOCL_UI_OPAQUECONTAINER_DRAW
			fprintf( stderr, "OpaqueContainer::draw: redraw changed\n" );
#endif

			this->Container::draw( gc );
		}
		gc.popClip();
	}
	gc.setBackground( original_background );
	
	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}




const Color&
OpaqueContainer::getBackgroundColor() const
{
	return this->bgColor;
}

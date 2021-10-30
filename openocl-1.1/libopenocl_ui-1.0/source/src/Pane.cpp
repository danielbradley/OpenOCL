/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Pane.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>
#include "openocl/ui/GraphicsContext.h"

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;

Pane::Pane()
{
	this->paneWidth  = 200;
	this->paneHeight = 200;
}

Pane::Pane( unsigned int width, unsigned int height )
{
	this->paneWidth  = width;
	this->paneHeight = height;
}

Pane::~Pane()
{}

void
Pane::setPaneWidth( unsigned int width )
{
	this->paneWidth = width;
}

void
Pane::setPaneHeight( unsigned int height )
{
	this->paneHeight = height;
}

Dimensions
Pane::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d;
	d.width  = Math::min( width, this->paneWidth );
	d.height = Math::min( height, this->paneHeight );

	return d;
}


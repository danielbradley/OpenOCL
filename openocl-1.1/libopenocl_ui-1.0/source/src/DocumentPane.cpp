/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/DocumentPane.h"

#include "openocl/ui/Container.h"
#include "openocl/ui/DirectedLayout.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"
#include <openocl/base/String.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;

unsigned int DocumentPane::padding = 5;

DocumentPane::DocumentPane() : Pane( 0xFFFF, 0xFFFF )
{
	this->bounds   = new Region();
	this->setLayout( new DirectedLayout( LayoutManager::EASTWARD ) );
}

DocumentPane::~DocumentPane()
{
	delete this->bounds;
}

bool
DocumentPane::draw( GraphicsContext& gc ) const
{
#ifdef DEBUG_OPENOCL_UI_DOCUMENTPANE
	fprintf( stderr, "%s.DocumentPane::draw( gc )\n", this->getName().getChars() );
#endif

	bool draw_again = this->Pane::draw( gc );
	return draw_again;
}

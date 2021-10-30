/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/carbon/CarbonApplicationMenu.h"
#include "openocl/ui/carbon/CarbonDisplay.h"
#include "openocl/ui/carbon/CarbonPanel.h"
#include "openocl/ui/carbon/CarbonVisual.h"

#include <Carbon/Carbon.h>

using namespace openocl::ui;
using namespace openocl::ui::carbon;

CarbonVisual::CarbonVisual()
{
	this->display = new CarbonDisplay();
}

CarbonVisual::~CarbonVisual()
{}

//-----------------------------------------------------------
//		public virtual methods (Visual)
//-----------------------------------------------------------

ApplicationMenu*
CarbonVisual::createApplicationMenu() const
{
	return new CarbonApplicationMenu();
}

Panel*
CarbonVisual::createPanel( flags type ) const
{
	return new CarbonPanel( *this, type );
}

void
CarbonVisual::enterEventLoop( bool* run ) const
{
	this->display->enterEventLoop( run, 0 );
}

void
CarbonVisual::enterEventLoop( bool* run, unsigned long winId ) const
{
	this->display->enterEventLoop( run, winId );
}

const AbstractDisplay&
CarbonVisual::getDisplay() const
{
	return *this->display;
}

const CarbonDisplay&
CarbonVisual::getCarbonDisplay() const
{
	return *this->display;
}

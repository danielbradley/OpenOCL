/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/carbon/CarbonDisplay.h"
#include "openocl/ui/carbon/CarbonEventManager.h"
#include <openocl/mvc/Controller.h>

using namespace openocl::base::event;
using namespace openocl::mvc;
using namespace openocl::ui;
using namespace openocl::ui::carbon;

CarbonDisplay::CarbonDisplay()
{
	this->eventManager = new CarbonEventManager();
}

CarbonDisplay::~CarbonDisplay()
{}

Panel*
CarbonDisplay::createPanel( flags type ) const
{
	return null;
}

EventSource&
CarbonDisplay::getEventSource() const
{
	return *this->eventManager;
}

Controller&
CarbonDisplay::getController() const
{
	return *this->eventManager;
}

unsigned int
CarbonDisplay::getDefaultScreen() const
{
	return 0;
}

unsigned int
CarbonDisplay::getWidthOf( unsigned int screenNum ) const
{
	return 1280; // XXX
}

unsigned int
CarbonDisplay::getHeightOf( unsigned int screenNum ) const
{
	return 1024; // XXX
}

unsigned int
CarbonDisplay::getWidthMMOf( unsigned int screenNum ) const
{
	return 0; // XXX
}

unsigned int
CarbonDisplay::getHeightMMOf( unsigned int screenNum ) const
{
	return 0; // XXX
}

void
CarbonDisplay::enterEventLoop( bool* run, unsigned long winId ) const
{
	this->eventManager->enterEventLoop( run, winId );
}

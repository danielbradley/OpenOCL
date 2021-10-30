/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/win32/WinPanel.h"
#include "openocl/ui/win32/WinScreen.h"

using namespace openocl::ui;
using namespace openocl::ui::win32;

WinScreen::WinScreen( WinVisual& aVisual, unsigned int aScreenId )
: winvisual( aVisual ), screenId( aScreenId )
{
	RECT work_area;

	SystemParametersInfo( SPI_GETWORKAREA, 0, &work_area, 0 ); 

	int clientX = work_area.left;
	int clientY = work_area.top;
	clientScreen.width   = work_area.right - work_area.left;
	clientScreen.height  = work_area.bottom - work_area.top;
	primaryScreen.width  = GetSystemMetrics( SM_CXSCREEN );
	primaryScreen.height = GetSystemMetrics( SM_CYSCREEN );
    virtualScreen.width  = GetSystemMetrics( SM_CXVIRTUALSCREEN );
    virtualScreen.height = GetSystemMetrics( SM_CYVIRTUALSCREEN );
}

Panel*
WinScreen::createPanel( int type )
{
	return new WinPanel( *this, type, this->screenId );
}

Dimensions
WinScreen::getScreenDimensions() const
{
	return this->primaryScreen;
}

unsigned long
WinScreen::getScreenDepth() const
{
	return 32;
}

unsigned long
WinScreen::getScreenId() const
{
	return this->screenId;
}

WinVisual&
WinScreen::getWinVisual() const
{
	return this->winvisual;
}


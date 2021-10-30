/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Panel.h"
#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/X11/XScreen.h"
#include "openocl/ui/X11/XVisual.h"
#include <X11/Xlib.h>

using namespace openocl::ui;
using namespace openocl::ui::X11;

XScreen::XScreen( XVisual& aVisual, unsigned int aScreenId )
: Screen(), xvisual( aVisual ), screenId( aScreenId )
{
	Display* dpy = (Display*) this->xvisual.getXDisplay();
	this->xdisplay = dpy;
	this->rootWindowId = RootWindow( dpy, aScreenId );

	this->screenDimensions.width  = 1280;
	this->screenDimensions.height = 1024;
	this->depth = DefaultDepth( dpy, aScreenId );
}

XScreen::~XScreen()
{}

//---------------------------------------------------------------------------
//	public virtual methods
//---------------------------------------------------------------------------

Panel*
XScreen::createPanel( int type )
{
	return new XPanel( *this, type, this->rootWindowId );
}

//---------------------------------------------------------------------------
//	public virtual constant methods
//---------------------------------------------------------------------------

Dimensions
XScreen::getScreenDimensions() const
{
	return screenDimensions;
}

unsigned long
XScreen::getScreenDepth() const
{
	return depth;
}

unsigned long
XScreen::getScreenId() const
{
	return this->screenId;
}

//-------------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------------

XVisual&
XScreen::getXVisual() const
{
	return this->xvisual;
}

void*
XScreen::getXDisplay() const
{
	return this->xdisplay;
}

unsigned long
XScreen::getRootWindowId() const
{
	return this->rootWindowId;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/X11/XVisual.h"

#include "openocl/ui/OffscreenImageManager.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/X11/XEventManager.h"
#include "openocl/ui/X11/XFontManager.h"
#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/X11/XScreen.h"
#include <openocl/base/event/EventSource.h>
//#include <openocl/mvc/Controller.h>

#include <cstdlib>
#include <cstdio>
#include <X11/Xlib.h>

using namespace openocl::base::event;
//using namespace openocl::mvc;
using namespace openocl::ui;
using namespace openocl::ui::X11;
using namespace openocl::util;

XVisual::XVisual() : Visual(), xscreen( null ), fontManager( null ), imageManager( null ), eventManager( null )
{
	if ( XInitThreads() )
	{
		Display* dpy = XOpenDisplay( null );
		if ( dpy )
		{
			XSynchronize( dpy, false );

			unsigned int default_screen_id = DefaultScreen( dpy );

			this->xdisplay = dpy;
			this->xscreen = new XScreen( *this, default_screen_id );
			this->fontManager  = new XFontManager( (void*) dpy );
			this->imageManager = new OffscreenImageManager();
			this->eventManager = new XEventManager( (void*) dpy );

			this->rootWindowId = RootWindow( dpy, default_screen_id );
		}
		else
		{
			fprintf( stderr, "XVisual: could not find X\n" );
			abort();
		}
	} else {
		fprintf( stderr, "XVisual: could not init threading support\n" );
		abort();
	}
}

XVisual::~XVisual()
{
	delete this->fontManager;
	delete this->imageManager;
	delete this->eventManager;
	delete this->xscreen;
	XCloseDisplay( (Display*) this->xdisplay );
}

//------------------------------------------------------------------------
//	public virtual methods
//------------------------------------------------------------------------

//Controller&
//XVisual::getController()
//{
//	return *this->eventManager;
//}

openocl::ui::Screen&
XVisual::getDefaultScreen()
{
	return *this->xscreen;
}

EventSource&
XVisual::getEventSource() const
{
	return *this->eventManager;
}

FontManager&
XVisual::getFontManager() const
{
	return *this->fontManager;
}

OffscreenImageManager&
XVisual::getOffscreenImageManager() const
{
	return *this->imageManager;
}

void
XVisual::enterEventLoop( bool* visible ) const
{
	this->eventManager->enterEventLoop( visible, 0 );
}

void
XVisual::enterEventLoop( bool* visible, unsigned long winId ) const
{
	this->eventManager->enterEventLoop( visible, winId );
}

void*
XVisual::getXDisplay() const
{
	return this->xdisplay;
}


ApplicationMenu*
XVisual::createApplicationMenu() const
{
	return null;
}

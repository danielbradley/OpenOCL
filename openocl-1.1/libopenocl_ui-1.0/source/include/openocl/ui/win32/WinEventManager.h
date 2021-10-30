/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINEVENTMANAGER_H
#define OPENOCL_UI_WIN32_WINEVENTMANAGER_H

#include <openocl/base/Object.h>
#include <openocl/base/event/EventSource.h>
#include <openocl/base/event.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {
		namespace win32 {

//LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

class WinEventManager
: public openocl::base::Object, //public openocl::mvc::Controller,
  public openocl::base::event::EventSource
{
public:
	WinEventManager();
	virtual ~WinEventManager();

	void registerDispatcher( void* id, openocl::base::event::EventDispatcher& anEventDispatcher );
	void deregisterDispatcher( void* id );

	void postEvent( openocl::base::event::Event* anEvent );
	void enterEventLoop( bool* loop, unsigned long winId );
	void exitEventLoop();

	static void SendCustomEvent( void* hwnd, mask eventType );

	//	public static methods called from Window Procedure

	static WinEventManager* eventManager;

	int nrOfWindows;
private:
	openocl::util::Index* dispatchers;
	bool masterLoop;
};

};};};

#endif

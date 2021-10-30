/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/OffscreenImageManager.h"
#include "openocl/ui/win32/WinEventManager.h"
#include "openocl/ui/win32/WinFontManager.h"
#include "openocl/ui/win32/WinPanel.h"
#include "openocl/ui/win32/WinScreen.h"
#include "openocl/ui/win32/WinVisual.h"
#include <openocl/base/event/EventSource.h>
//#include <openocl/mvc/Controller.h>

//#include <cstdlib>
#include <cstdio>

using namespace openocl::base::event;
//using namespace openocl::mvc;
//using namespace openocl::ui;
using namespace openocl::ui::win32;

WinVisual* WinVisual::instance = null;

extern LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );
extern LRESULT CALLBACK PopupWindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

WinVisual::WinVisual()
{
	//	According to unreliable sources on the web this is supposed to
	//	be equivalent to the hInstance passed to WinMain.
	this->hInstance     = GetModuleHandle( NULL );	//	Returns address of current DLL (0x400000)
	this->fontManager   = new WinFontManager();
	this->eventManager  = new WinEventManager();
	this->imageManager  = new openocl::ui::OffscreenImageManager();
	this->defaultScreen = new WinScreen( *this, 0 );
	this->windowsUsed = 0;
	this->buffersUsed = 0;

	//this->windisplay = new WinDisplay( this );



	//
	//	Taken from MSDN Using Window Classes
	//
	WNDCLASSEX wcx;	
	wcx.cbSize = sizeof( WNDCLASSEX );					//	size of structure
	wcx.style =  CS_HREDRAW | CS_VREDRAW;				//	redraw if size changes
	wcx.style =  CS_SAVEBITS;							//	save screen under window
	wcx.style |= CS_DBLCLKS;							//	receive double clicks

	wcx.lpfnWndProc = WindowProcedure;					//	points to window procedure
	wcx.cbClsExtra = 0;									//	no extra class memory
	wcx.cbWndExtra = 0;									//	no extra window memory
	wcx.hInstance = (HINSTANCE) hInstance;							//	handle to instance
	wcx.hIcon = LoadIcon( NULL, IDI_APPLICATION );		//	predefined app. icon
	wcx.hCursor = LoadCursor( NULL, IDC_ARROW );		//	predefined arrow
	wcx.hbrBackground = (HBRUSH) GetStockObject( WHITE_BRUSH );	//	white background brush
	wcx.lpszMenuName = "MainMenu";
	wcx.lpszClassName = "OpenOCL_Managed";
	wcx.hIconSm = (HICON) LoadImage( (HINSTANCE) hInstance, MAKEINTRESOURCE(5), IMAGE_ICON,
		GetSystemMetrics(SM_CXSMICON),
		GetSystemMetrics(SM_CXSMICON),
		LR_DEFAULTCOLOR );

	//	Register Default class
	RegisterClassEx( &wcx );

	//	Register Menu class
	wcx.lpszClassName = "OpenOCL_Popup";
	wcx.lpfnWndProc = PopupWindowProcedure;
	//wcx.style |= Button;
	RegisterClassEx( &wcx );
}

WinVisual::~WinVisual()
{
	delete this->fontManager;
	delete this->eventManager;
	delete this->imageManager;
	delete this->defaultScreen;

	//PostQuitMessage( 0 );
}

//-----------------------------------------------------------
//		public virtual methods (Visual)
//-----------------------------------------------------------

openocl::ui::Screen&
WinVisual::getDefaultScreen()
{
	return *this->defaultScreen;
}

openocl::ui::FontManager&
WinVisual::getFontManager() const
{
	return *this->fontManager;
}

openocl::ui::OffscreenImageManager&
WinVisual::getOffscreenImageManager() const
{
	return *this->imageManager;
}

/*
Controller&
WinVisual::getController()
{
	return *this->eventManager;
}
*/

EventSource&
WinVisual::getEventSource() const
{
	return *this->eventManager;
}

openocl::ui::Panel*
WinVisual::createPanel( flags type )
{
	return new WinPanel( *this->defaultScreen, type, 0 );
}

void
WinVisual::enterEventLoop( bool* run ) const
{
	this->eventManager->enterEventLoop( run, 0 );
}

void
WinVisual::enterEventLoop( bool* run, unsigned long winId ) const
{
	this->eventManager->enterEventLoop( run, winId );
}

//const AbstractDisplay&
//WinVisual::getDefaultDisplay() const
//{
//	fprintf( stderr, "WinVisual::getDefaultDisplay deprecated\n" );
//	abort();
//}

//const AbstractDisplay&
//WinVisual::getDisplay() const
//{
//	fprintf( stderr, "WinVisual::getDisplay deprecated\n" );
//	abort();
//}

//const WinDisplay&
//WinVisual::getWinDisplay() const
//{
//	fprintf( stderr, "WinVisual::getWinDisplay deprecated\n" );
//	abort();
//}

openocl::ui::ApplicationMenu*
WinVisual::createApplicationMenu() const
{
	return null;
}

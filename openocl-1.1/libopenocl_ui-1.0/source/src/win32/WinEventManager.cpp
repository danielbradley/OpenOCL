/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/win32/WindowsUndefs.h"

#include "openocl/ui/win32/WinEventManager.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/CloseEvent.h>
#include <openocl/base/event/ConfigureEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventDispatcher.h>
#include <openocl/base/event/ExposeEvent.h>
#include <openocl/base/event/KeyDownEvent.h>
#include <openocl/base/event/KeyUpEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/base/event/RefreshEvent.h>
#include <openocl/base/event/RedrawEvent.h>
#include <openocl/base/event/RelayoutEvent.h>
#include <openocl/base/event/RepositionEvent.h>
#include <openocl/base/event/ResizeEvent.h>
#include <openocl/util/Index.h>
#include <openocl/util/MemoryPointerKey.h>
#include <openocl/util/NumberKey.h>

#include <cstdio>

//#define OPENOCL_UI_WIN32_WINEVENTMANAGER_DEBUG_KNOWNEVENTS
//#define OPENOCL_UI_WIN32_WINEVENTMANAGER_DEBUG_UNKNOWNEVENTS

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;
using namespace openocl::ui::win32;
using namespace openocl::util;

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 522
#endif

static void processKeyboardMessage( MSG msg );
static void processKeyMessage( MSG msg );
static void processCharacterMessage( MSG msg );
static void processMouseMessage( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );


WinEventManager* WinEventManager::eventManager;

WinEventManager::WinEventManager()
{
	this->masterLoop = true;
	this->dispatchers = new Index();
	WinEventManager::eventManager = this;
	this->nrOfWindows = 0;
}

WinEventManager::~WinEventManager()
{
	delete this->dispatchers;
}

void
WinEventManager::registerDispatcher( void* id, EventDispatcher& anEventDispatcher )
{
	MemoryPointerKey key( id );
	if ( this->dispatchers->put( key, &anEventDispatcher ) )
	{
		fprintf( stderr, "WinEventManager::registerDispatcher(): attempting to register registered window\n" );
		abort();
	}
}

void
WinEventManager::deregisterDispatcher( void* id )
{
	MemoryPointerKey key( id );
	try
	{
		this->dispatchers->removeValue( key );
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		fprintf( stderr, "WinEventManager::deregisterDispatcher(): attempting to unregister unregistered window\n" );
		abort();
	}
}

void
WinEventManager::postEvent( Event* anEvent )
{
	unsigned long target = anEvent->getTargetId();

	MemoryPointerKey targetKey( (void*) target );
	try
	{
		void* dispatcher = const_cast<void*>( this->dispatchers->getValue( targetKey ) );
		EventDispatcher* e = reinterpret_cast<EventDispatcher*>( dispatcher );
		e->dispatchEvent( anEvent );
	} catch ( NoSuchElementException* ex ) {

		//	It may be possible for trailing events to be sent to
		//	the window after it has been deregistered. This shouldn't happen
		//	so abort if it does.

		delete ex;
		delete anEvent;

		//	Abort() has been commented out as popup windows will cause events
		//	to be send to the window before it has been registered with the
		//	event manager.
		//
		//	abort();
	}
}

/*
void
WinEventManager::enterEventLoop( bool* loop, unsigned long winId )
{
	MSG msg;

	unsigned int nr = 0;

	while ( *loop && GetMessage( &msg, (HWND) winId, 0, 0 ) )
	{
		//fprintf( stderr, "\nEVENT LOOP\n" );

		switch ( msg.message )
		{
		case WM_KEYDOWN:
		case WM_KEYUP:
			//	Called TranslateMessage which produces a CharMessage
			TranslateMessage( &msg );
			break;
		case WM_CHAR:
			processCharacterMessage( msg );
			break;
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			processMouseMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			break;
		default:
			TranslateMessage( &msg );
			DispatchMessage( &msg );	// Dispatches message to appropriate
		}								// windows procedure.
		nr++;
	}
}
*/

/**
 *	@param loop, pointer to boolean variable that can be modified
 *  to cause this loop to end (example Dialog).
 *
 *  @param winId, if 0 events for all windows should be processed,
 *                else indicates that a window is modal and that
 *                only events for that window should be processed,
 *                except for window move, and window minimize, maximize
 *                events for the parent.
 */

void
WinEventManager::enterEventLoop( bool* loop, unsigned long winId )
{
	HWND win_id = (HWND) winId;
	MSG msg;

	if ( 0 == winId )
	{
		this->masterLoop = true;
	}

	unsigned int nr = 0;
	while ( this->masterLoop && *loop && GetMessage( &msg, NULL, 0, 0 ) )
	{
		//fprintf( stderr, "\nEVENT LOOP\n" );

		switch ( msg.message )
		{
		case WM_KEYUP:
		case WM_KEYDOWN:
			if ( !winId || (win_id == msg.hwnd) )
			{
				switch ( msg.wParam )
				{
				case VK_LEFT:
				case VK_RIGHT:
				case VK_UP:
				case VK_DOWN:
				case VK_DELETE:
					processKeyMessage( msg );
					break;
				default:
					//	Called TranslateMessage which produces a CharMessage
					TranslateMessage( &msg );
				}
			}	
			break;
		case WM_CHAR:
			if ( !winId || (win_id == msg.hwnd) )
			{
				processCharacterMessage( msg );
			}	
			break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			//fprintf( stderr, "WinEventManager::enterEventLoop: button\n" );
		case WM_MOUSEMOVE:
		case WM_MOUSEWHEEL:
			//if ( !winId || (win_id == msg.hwnd) )
			{
				processMouseMessage( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			}
			//else
			//{
			//	DefWindowProc( msg.hwnd, msg.message, msg.wParam, msg.lParam );
			//}
			break;
		default:
			//if ( !winId || !msg.hwnd || (win_id == msg.hwnd) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );	// Dispatches message to appropriate
			}
		}									// windows procedure.
		nr++;
	}
	//fprintf( stderr, "WinEventManager::exitEventLoop(): loop = %x\n", this->masterLoop );
}

void
WinEventManager::exitEventLoop()
{
	this->masterLoop = false;
}

void
WinEventManager::SendCustomEvent( void* hwnd, mask eventType )
{
	HWND window_id = (HWND) hwnd;

	unsigned int message = 0;
	switch ( eventType )
	{
	case Event::REFRESH_EVENT:
		message = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REFRESH" );
		break;
	case Event::RELAYOUT_EVENT:
		message = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_RELAYOUT" );
		break;
	case Event::REDRAW_EVENT:
		message = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REDRAW" );
		break;
	case Event::EXPOSE_EVENT:
		message = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_EXPOSE" );
		break;
	default:
		return;
	}

	PostMessage( window_id, message, 0, 0 );
}

static void processKeyboardMessage( MSG msg )
{
	Event* event = null;
	const Object& eventSource = *WinEventManager::eventManager;
	unsigned long target_id = (unsigned long) msg.hwnd;
	unsigned int key;

	switch ( msg.message )
	{
	case WM_KEYDOWN:
		//fprintf( stderr, "KEYDOWN\n" );
		break;
	case WM_KEYUP:
		//fprintf( stderr, "KEYUP\n" );
		break;
	}
	//event = new KeyEvent( event_source, target_id, x, y, modifiers, key )
}

static void processKeyMessage( MSG msg )
{
	Event* event = null;
	const Object& event_source = *WinEventManager::eventManager;
	unsigned long target_id = (unsigned long) msg.hwnd;
	int x = 0;
	int y = 0;

	unsigned int key = msg.wParam;
	flags modifiers = 0;

	switch ( msg.wParam )
	{
	case VK_LEFT:
		key = KeyEvent::LEFT;
		break;
	case VK_RIGHT:
		key = KeyEvent::RIGHT;
		break;
	case VK_UP:
		key = KeyEvent::UP;
		break;
	case VK_DOWN:
		key = KeyEvent::DOWN;
		break;
	case VK_DELETE:
		key = KeyEvent::DELETE;
		break;
	//case VK_HOME:
	//	key = KeyEvent::HOME;
	//	break;
	//case VK_END:
	//	key = KeyEvent::END;
	//	break;
	//case VK_INSERT:
	//	key = KeyEvent::INSERT;
	//	break;
	}

	//	If the high order bit is set the key is pressed.
	if ( 0x8000 & GetKeyState( VK_SHIFT ) )
	{
		modifiers |= InputEvent::SHIFT;
	}
	if ( 0x8000 & GetKeyState( VK_CONTROL ) )
	{
		modifiers |= InputEvent::CTRL;
	}

	switch ( msg.message )
	{
	case WM_KEYUP:
		event = new KeyUpEvent( event_source, target_id, x, y, modifiers, key );
		WinEventManager::eventManager->postEvent( event );
		break;
	case WM_KEYDOWN:
		event = new KeyDownEvent( event_source, target_id, x, y, modifiers, key );
		WinEventManager::eventManager->postEvent( event );
		break;
	}
}

static void processCharacterMessage( MSG msg )
{
	Event* event = null;
	const Object& event_source = *WinEventManager::eventManager;
	unsigned long target_id = (unsigned long) msg.hwnd;
	unsigned int key = msg.wParam;
	int x = 0;
	int y = 0;
	flags modifiers = 0;

	//	If the high order bit is set the key is pressed.
	if ( 0x8000 & GetKeyState( VK_SHIFT ) )
	{
		modifiers |= InputEvent::SHIFT;
	}
	if ( 0x8000 & GetKeyState( VK_CONTROL ) )
	{
		modifiers |= InputEvent::CTRL;
	}

	switch ( 0x8000 & msg.lParam )
	{
	case 0x8000:
		event = new KeyUpEvent( event_source, target_id, x, y, modifiers, key );
	default:
		event = new KeyDownEvent( event_source, target_id, x, y, modifiers, key );
	}
	WinEventManager::eventManager->postEvent( event );
}

static void processMouseMessage( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	unsigned long target_id = (unsigned long) hwnd;
	unsigned int click_count = 0;
	flags        button      = 0;
	flags        state       = 0;
	flags        modifiers   = 0;

	//
	//	Determine pointer position (MotionEvent and MouseEvent)

	int distance = (int)(short)HIWORD(wParam);

	int x = (int)(short)LOWORD(lParam);
	int y = (int)(short)HIWORD(lParam);

	//
	//	Deterine click count (MouseEvent)

	switch ( message )
	{
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		click_count = 1;
		break;
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		click_count = 2;
	}

	//
	//	Determine keyboard modifiers and existing button
	//	state (MotionEvent and MouseEvent)

	if ( MK_CONTROL == (MK_CONTROL & wParam))
	{
		modifiers |= InputEvent::CTRL;
	}
	if ( MK_SHIFT == (MK_SHIFT & wParam))
	{
		modifiers |= InputEvent::SHIFT;
	}

	if ( MK_LBUTTON == (MK_LBUTTON & wParam))
	{
		state = state |= PointerEvent::BUTTON1;
	}
	if ( MK_MBUTTON == (MK_MBUTTON & wParam))
	{
		state = state |= PointerEvent::BUTTON2;
	}
	if ( MK_RBUTTON == (MK_RBUTTON & wParam))
	{
		state = state |= PointerEvent::BUTTON3;
	}
	//if ( MK_XBUTTON1 == (MK_XBUTTON1 & wParam))
	//{
	//	state = state |= PointerEvent::BUTTON4;
	//}
	//if ( MK_XBUTTON2 == (MK_XBUTTON2 & wParam))
	//{
	//	state = state |= PointerEvent::BUTTON5;
	//}

	//
	//	Determine mouse button pressed, if any.

	switch ( message )
	{
	case WM_LBUTTONDOWN:
		button = MouseEvent::BUTTON1_PRESS;
		break;
	case WM_MBUTTONDOWN:
		button = MouseEvent::BUTTON2_PRESS;
		break;
	case WM_RBUTTONDOWN:
		button = MouseEvent::BUTTON3_PRESS;
		break;
	case WM_LBUTTONUP:
		button = MouseEvent::BUTTON1_RELEASE;
		break;
	case WM_MBUTTONUP:
		button = MouseEvent::BUTTON2_RELEASE;
		break;
	case WM_RBUTTONUP:
		button = MouseEvent::BUTTON3_RELEASE;
		break;
	case WM_LBUTTONDBLCLK:
		button = MouseEvent::BUTTON1_PRESS;
//		button = MouseEvent::BUTTON1_RELEASE;
		break;
	case WM_MBUTTONDBLCLK:
		button = MouseEvent::BUTTON2_PRESS;
//		button = MouseEvent::BUTTON2_RELEASE;
		break;
	case WM_RBUTTONDBLCLK:
		button = MouseEvent::BUTTON3_PRESS;
//		button = MouseEvent::BUTTON3_RELEASE;
		break;
	default:
		if ( distance > 0 )
		{
			button = MouseEvent::BUTTON4_PRESS;
		}
		else if ( distance < 0 )
		{
			button = MouseEvent::BUTTON5_PRESS;
		}
	}

	Event* event;
	switch ( message )
	{
	case WM_MOUSEMOVE:
		event = new MotionEvent( *WinEventManager::eventManager, target_id, x, y, modifiers, state );
		break;
	default:
		event = new MouseEvent( *WinEventManager::eventManager, target_id, x, y, modifiers, state, button, click_count );
	}
	WinEventManager::eventManager->postEvent( event );
}

const char*
GetMessageName( unsigned int message )
{
	const char* name = null;

	unsigned int refresh_event  = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REFRESH" );
	unsigned int relayout_event = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_RELAYOUT" );
	unsigned int redraw_event   = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REDRAW" );
	unsigned int expose_event   = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_EXPOSE" );

	if ( message == refresh_event )
	{
		name = "OPENOCL_UI_WIN32_REFRESH\n";
	}
	else if ( message == relayout_event )
	{
		name = "OPENOCL_UI_WIN32_RELAYOUT\n";
	}
	else if ( message == redraw_event )
	{
		name = "OPENOCL_UI_WIN32_REDRAW\n";
	}
	else if ( message == expose_event )
	{
		name = "OPENOCL_UI_WIN32_EXPOSE\n";
	}
	else
	{
		switch ( message )
		{
		case WM_ACTIVATE: // 0x0006 (6)
			name = "WM_ACTIVATE\n";
			break;
		case WM_SETFOCUS: //0x0007 (7)
			name = "WM_SETFOCUS\n";
			break;
		case WM_KILLFOCUS: // 0x0008 (8)
			name = "WM_KILLFOCUS\n";
			break;
		case WM_SETTEXT: // 0x000C (12)
			name = "WM_SETTEXT\n";
			break;
		case WM_GETMINMAXINFO: // 0x0024 (36)
			name = "WM_GETMINMAXINFO\n";
			break;
		case WM_NCCREATE: //0x0081 (129)
			name = "WM_NCCREATE\n";
			break;
		case WM_NCCALCSIZE:
			name = "WM_NCCALCSIZE\n";
			break;
		case WM_ERASEBKGND: // 0x0014 (20)
			name = "WM_ERASEBKGND\n";
			break;
		case WM_WINDOWPOSCHANGING: // 0x0046 (70)
			name = "WM_WINDOWPOSCHANGING\n";
			break;
		case WM_GETTEXT: // 0x000D (13):
			name = "WM_GETTEXT\n";
			break;
		case WM_SHOWWINDOW: // 0x0018 (24)
			name = "WM_SHOWWINDOW\n";
			break;
		case WM_ACTIVATEAPP: // 0x001C (28)
			name = "WM_ACTIVATEAPP\n";
			break;
		case WM_SETCURSOR: // 0x0020 (32)
			name = ""; //WM_SETCURSOR\n";
			break;
		case WM_GETICON: //0x007F (127)
			name = "WM_GETICON\n";
			break;
		case WM_NCACTIVATE: // 0x0086 (134)
			name = "WM_NCACTIVATE\n";
			break;
		case WM_NCHITTEST: //132 // 0x84
			name = ""; //WM_NCHITTEST\n";
			break;
		case WM_SYNCPAINT: //0x0088 (136)
			name = "WM_SYNCPAINT\n";
			break;
		case WM_NCMOUSEMOVE: // 0x00a0 (160)
			name = "WM_NCMOUSEMOVE\n";
			break;
		case WM_CREATE:
			name = "WM_CREATE\n";
			break;
		case WM_DESTROY:
			name = "WM_DESTROY\n";
			break;
		case WM_NCDESTROY:
			name = "WM_NCDESTORY\n";
			break;
		case WM_WINDOWPOSCHANGED:
			name = "WM_WINDOWPOSCHANGED\n";
			break;
		case WM_MOVE:
			name = "WM_MOVE\n";
			break;
		case WM_SIZE:
			name = "WM_SIZE\n";
			break;
		case WM_PAINT:
			name = "WM_PAINT\n";
			break;
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			name = "BUTTONS\n";
			break;
		case WM_KEYDOWN:
		case WM_KEYUP:
			name = "KEYS\n";
			break;
		case WM_NCPAINT: //0x0085 (133)
			name = "WM_NCPAINT\n";
			break;
		case WM_MOUSEACTIVATE: //0x0021 (33)
			name = "WM_MOUSEACTIVATE\n";
			break;
		case WM_SYSCOMMAND: // 0x0112 (274)
			name = "WM_SYSCOMMAND\n";
			break;
		case WM_NCLBUTTONDOWN: // 0x00A1 (161)
			name = "WM_MOUSEACTIVATE\n";
			break;
		case WM_SIZING: // 0x0214 (532)
			name = "WM_SIZING\n";
			break;
		case WM_CAPTURECHANGED: // 0x0215 (533)
			name = "WM_CAPTURECHANGED\n";
			break;
		case WM_ENTERSIZEMOVE: // 0x0231 (561)
			name = "WM_ENTERSIZEMOVE\n";
			break;
		case WM_EXITSIZEMOVE: // 0x0232 (562)
			name = "WM_EXITSIZEMOVE\n";
			break;
		case WM_CLOSE: // 0x0010 (16)
			name = "WM_CLOSE\n";
			break;
		default:
			name = "UNKNOWN\n";
		}
	}
	return name;
}

LRESULT CALLBACK WindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	unsigned int refresh_event  = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REFRESH" );
	unsigned int relayout_event = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_RELAYOUT" );
	unsigned int redraw_event   = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_REDRAW" );
	unsigned int expose_event   = RegisterWindowMessage( "OPENOCL_UI_WIN32_WINEVENTMANAGER_EXPOSE" );

	//fprintf( stderr, "\nWINDOW PROCEDURE\n" );

	//fprintf( stderr, "%s", GetMessageName( message ) );

	switch ( message )
	{
	case WM_ERASEBKGND: // 0x0014 (20)
		{
			Event* e = new ChangeEvent( *WinEventManager::eventManager, (unsigned long) hwnd, ChangeEvent::REDRAW_ALL );
			WinEventManager::eventManager->postEvent( e );
		}
		break;
	case WM_SETFOCUS: //0x0007 (7)
		break;
	case WM_KILLFOCUS: // 0x0008 (8)
		//	When window is told to minimize
		break;
	case WM_CLOSE:
		//	When window should close (proceeded by 161,533,274 if by close button?)
		{
			CloseEvent* e = new CloseEvent( *WinEventManager::eventManager, (unsigned long) hwnd );
			WinEventManager::eventManager->postEvent( e );
			return 0;
		}
		break;
	case WM_CREATE:
		{
			WinEventManager* em = WinEventManager::eventManager;
			em->nrOfWindows++;
		}
		//	Fall-through to default handler
		break;
	case WM_NCDESTROY:
		//fprintf( stderr, "WINDOW_PROCEDURE: WM_NCDESTROY\n" );
		{
			WinEventManager* em = WinEventManager::eventManager;
			em->nrOfWindows--;
			if ( 0 == WinEventManager::eventManager->nrOfWindows )
			{
				//fprintf( stderr, "WINDOW_PROCEDURE(): %i windows left, exiting\n", em->nrOfWindows ); 
				WinEventManager::eventManager->exitEventLoop();
				//PostQuitMessage( 0 );
			}
			//return DefWindowProc( hwnd, message, wParam, lParam );
		}
		//	Fall-through to default handler
		break;
	case WM_WINDOWPOSCHANGED:
		{
			WINDOWPOS window_position = *((WINDOWPOS*) lParam);
			if ( !window_position.flags )
			{
				fprintf( stderr, "WindowProcedure(): Window Position Changed: NULL flags\n" );
			}

			Event* e = new RepositionEvent( *WinEventManager::eventManager,
				                            (unsigned long) hwnd,
											window_position.x,
											window_position.y,
			                                (unsigned long) window_position.hwndInsertAfter );
			WinEventManager::eventManager->postEvent( e );
		}
		break;
	case WM_MOVE:
		{
			//int x = (int)(short) LOWORD(lParam);
			//int y = (int)(short) HIWORD(lParam);
			//
			//Event* e = new RepositionEvent( *WinEventManager::eventManager,
			//	                            (unsigned long) hwnd,
			//								x,
			//								y,
			//                              (unsigned long) 0 );
			//WinEventManager::eventManager->postEvent( e );
		}
		break;
	case WM_SIZE:
		{
			int w = (int)(short) LOWORD(lParam);
			int h = (int)(short) HIWORD(lParam);

			Event* e = new ResizeEvent( *WinEventManager::eventManager,
					                    (unsigned long) hwnd,
									    w,
										h,
										0 );
			WinEventManager::eventManager->postEvent( e );
		}
		break;
	case WM_PAINT:
		//printf( "WindowProcedure(): Window Paint\n" );
		{
			unsigned long targetId = (unsigned long) hwnd;
			Event* e = new ExposeEvent( *WinEventManager::eventManager, targetId, 0, 0, 0, 0, 0 );
			WinEventManager::eventManager->postEvent( e );
		}
		return 0;
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
		fprintf( stderr, "WindowProcedure(): Unexpected unqueued event (MouseEvent) found in WindowProcedure\n" );
		processMouseMessage( hwnd, message, wParam, lParam );
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		fprintf( stderr, "WindowProcedure(): Unexpected unqueued event (KeyEvent) found in WindowProcedure\n" );
		return DefWindowProc( hwnd, message, wParam, lParam );
		break;
	case WM_MOUSEACTIVATE:
		break;
	default:
		{
			unsigned long targetId = (unsigned long) hwnd;
			Event* e = null;

			if ( message == refresh_event )
			{
				e = new RefreshEvent( *WinEventManager::eventManager, targetId );
				WinEventManager::eventManager->postEvent( e );
			}
			else if ( message == relayout_event )
			{
				e = new RelayoutEvent( *WinEventManager::eventManager, targetId, 0, 0, 0 );
				WinEventManager::eventManager->postEvent( e );
			}
			else if ( message == redraw_event )
			{
				e = new RedrawEvent( *WinEventManager::eventManager, targetId, 0, 0, 0 );
				WinEventManager::eventManager->postEvent( e );
			}
			else if ( message == expose_event )
			{
				e = new ExposeEvent( *WinEventManager::eventManager, targetId, 0, 0, 0, 0, 0 );
				WinEventManager::eventManager->postEvent( e );
			}
#ifdef OPENOCL_UI_WIN32_WINEVENTMANAGER_DEBUG_UNKNOWNEVENTS
			else
			{
				fprintf( stderr, "WinEventManager::WindowProcedure(): unknown( %i )\n", message );
			}
#endif
		}
	}
	return DefWindowProc( hwnd, message, wParam, lParam );
	//return 0;
}

LRESULT CALLBACK PopupWindowProcedure( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_MOUSEACTIVATE:
		return MA_NOACTIVATE;
	default:
		return WindowProcedure( hwnd, message, wParam, lParam );
	}
}

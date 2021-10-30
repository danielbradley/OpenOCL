/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Frame.h"
#include "openocl/ui/EventDispatcherHolder.h"
#include "openocl/ui/X11/XEventManager.h"
#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/X11/XVisual.h"

#include <openocl/base/Debug.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/CloseEvent.h>
#include <openocl/base/event/ConfigureEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventDispatcher.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event/ExposeEvent.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyDownEvent.h>
#include <openocl/base/event/KeyUpEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/RedrawEvent.h>
#include <openocl/base/event/RefreshEvent.h>
#include <openocl/base/event/RelayoutEvent.h>
#include <openocl/base/event/RepositionEvent.h>
#include <openocl/base/event/ResizeEvent.h>
#include <openocl/io/IO.h>
#include <openocl/threads/Mutex.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/MemoryPointerKey.h>
#include <openocl/util/NumberKey.h>
#include <openocl/util/Sequence.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <cstdlib>
#include <cstdio>

#define X_CLOSE      "WM_DELETE_WINDOW"
#define OCL_REFRESH  "OPENOCL_UI_X11_XEVENTMANAGER_REFRESH"
#define OCL_RELAYOUT "OPENOCL_UI_X11_XEVENTMANAGER_RELAYOUT"
#define OCL_REDRAW   "OPENOCL_UI_X11_XEVENTMANAGER_REDRAW"
#define OCL_EXPOSE   "OPENOCL_UI_X11_XEVENTMANAGER_EXPOSE"

#ifndef XK_ISO_LEFT_TAB
#define XK_ISO_LEFT_TAB 65056
#endif

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::ui::X11;
using namespace openocl::util;

const unsigned int XEventManager::EXPOSURE         = ExposureMask;
const unsigned int XEventManager::KEY_PRESS        = KeyPressMask;
const unsigned int XEventManager::BUTTON_PRESS     = ButtonPressMask;
const unsigned int XEventManager::STRUCTURE_NOTIFY = StructureNotifyMask;

openocl::threads::Mutex XEventManager::mutex;

//  Uncomment to enable debug statements or use cc -D
//  #define OPENOCL_UI_X11_XEVENTMANAGER_DEBUG



#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER

static void printXEvent( XEvent* event, Display* d )
{
	switch ( event->type )
	{
	case KeyPress:
		fprintf( stderr, "KeyPress\n" );
		break;
	case KeyRelease:
		fprintf( stderr, "KeyRelease\n" );
		break;
	case ButtonPress:
		fprintf( stderr, "ButtonPress\n" );
		break;
	case ButtonRelease:
		fprintf( stderr, "ButtonRelease\n" );
		break;
	case MotionNotify:
		//fprintf( stderr, "MotionNotify\n" );
		break;
	case ConfigureNotify:
		fprintf( stderr, "ConfigureNotify\n" );
		break;
	case CreateNotify:
		fprintf( stderr, "DestroyNotify" );
		break;
	case DestroyNotify:
		fprintf( stderr, "DestroyNotify" );
		break;
	case Expose:
		fprintf( stderr, "Expose\n" );
		break;
	case FocusIn:
		fprintf( stderr, "FocusIn\n" );
		break;
	case FocusOut:
		fprintf( stderr, "FocusOut\n" );
		break;
	case ClientMessage:
		fprintf( stderr, "ClientMessage: " );
		{
			const Atom close_atom    = XInternAtom( d, X_CLOSE, false );
			const Atom refresh_atom  = XInternAtom( d, OCL_REFRESH, 0 );
			const Atom relayout_atom = XInternAtom( d, OCL_RELAYOUT, 0 );
			const Atom redraw_atom   = XInternAtom( d, OCL_REDRAW, 0 );
			const Atom expose_atom   = XInternAtom( d, OCL_EXPOSE, 0 );

			XClientMessageEvent* cm = (XClientMessageEvent*) event;
			const Atom event_atom = (Atom) cm->data.l[0];

			if ( event_atom == close_atom )
			{
				fprintf( stderr, "WM_DELETE_WINDOW\n" );
			}
			else if ( event_atom == refresh_atom )
			{
				fprintf( stderr, "OPENOCL_UI_X11_XEVENTMANAGER_REFRESH\n" );
			}
			else if ( event_atom == relayout_atom )
			{
				fprintf( stderr, "OPENOCL_UI_X11_XEVENTMANAGER_RELAYOUT\n" );
			}
			else if ( event_atom == redraw_atom )
			{
				fprintf( stderr, "OPENOCL_UI_X11_XEVENTMANAGER_REDRAW\n" );
			}
			else if ( event_atom == expose_atom )
			{
				fprintf( stderr, "OPENOCL_UI_X11_XEVENTMANAGER_EXPOSE\n" );
			} else {
				fprintf( stderr, "UNKNOWN\n" );
			}
		}
		break;
	default:
		fprintf( stderr, "Unknown\n" );
	}
}
#endif

XEventManager::XEventManager( void* xdisplay )
: Object(), xdisplay( xdisplay )
{
  this->handlers = new Sequence();
  this->dispatchers = new Dictionary();

  this->countKeyPress = 0;
  this->countButtonPress = 0;
  this->countButtonRelease = 0;
  this->countMotion = 0;
  this->countExpose = 0;
  this->countConfigure = 0;
  this->countFocus = 0;
}

XEventManager::~XEventManager()
{
	delete this->handlers;
	delete this->dispatchers;
}

//void
//XEventManager::selectEvent( const XDisplay& aDisplay,
//                           const Frame& aFrame,
//                           flags eventTypes )
//{
//  Display* display = (Display*) aDisplay.display;
//  Window win = (Window) aFrame.window;
//
//  XSelectInput( display, win, eventTypes );
//}

// EventSource
void
XEventManager::registerDispatcher( void* id, EventDispatcher& anEventDispatcher )
{
	MemoryPointerKey* key = new MemoryPointerKey( id );
	//fprintf( stderr, "XEventManager::registerDispatcher( %x )\n", &anEventDispatcher );

	if ( this->dispatchers->put( key, new EventDispatcherHolder( anEventDispatcher ) ) )
	{
		abort();
	}
}

void
XEventManager::deregisterDispatcher( void* id )
{
	MemoryPointerKey key( id );
	try
	{
		//fprintf( stderr, "XEventManager::deregisterDispatcher( %x )\n", id );
		delete this->dispatchers->removeValue( key );
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		abort();
	}
}

// EventSource
void
XEventManager::postEvent( Event* anEvent )
{
	unsigned long target = anEvent->getTargetId();

	MemoryPointerKey key( (void*) target );
	try
	{
		//fprintf( stderr, "XEventManager::postEvent: retrieving EventDispatcher(%x)\n", target );
		const Object& object = this->dispatchers->getValue( key );
		Object& obj = const_cast<Object&>( object );
		EventDispatcherHolder& holder = dynamic_cast<EventDispatcherHolder&>( obj );
		holder.dispatcher.dispatchEvent( anEvent );
	}
	catch ( NoSuchElementException* ex )
	{

		//	It may be possible for trailing events to be sent to
		//	the window after it has been deregistered. This shouldn't happen
		//	so abort if it does.

		delete ex;
		delete anEvent;
#ifdef OPENOCL_UI_X11_XEVENTMANAGER_DEBUG
		fprintf( stderr, "XEventManager::postEvent: warning late event\n" );
#endif
		//abort();
	}	
}

//void
//EventManager::addEventHandler( EventHandler& anEventHandler )
//{
//  Object& obj = dynamic_cast<Object&>( anEventHandler );
//
//  this->handlers->add( obj );
//}

//void
//EventManager::fireEvent( Event* anEvent, long unsigned int id )
//{
//  IO::out().println( "firing events" );
//
//  Iterator* it = this->handlers->iterator();
//  while ( it->hasNext() )
//  {
//    const EventHandler* eh =
//        dynamic_cast<const EventHandler*>( &it->next() );
//    EventHandler* eh2 = (EventHandler*) eh;
//
//    if ( id == eh2->getEventHandlerId() )
//    {
//      IO::out().println( "sending event" );
//      eh2->dispatchEvent( anEvent );
//      break;
//    }
//  }
//}

#include <stdio.h>
void
XEventManager::enterEventLoop( bool* visible, unsigned long winId )
{
	Display* d = (Display*) this->xdisplay;
	XEvent event;
	XEvent next;
	XEvent reposition;
	XEvent resize;
	XEvent expose;

	while ( *visible )
	{
#ifndef OPENOCL_UI_X11_XEVENTMANAGER_NONPRIORITISED
		if (	XCheckTypedEvent( d, ButtonPress, &event )     ||
			XCheckTypedEvent( d, ButtonRelease, &event )   ||
			XCheckTypedEvent( d, KeyPress, &event )        ||
			XCheckTypedEvent( d, KeyRelease, &event )      ||
			XCheckTypedEvent( d, ConfigureNotify, &event ) ||
			XCheckTypedEvent( d, Expose, &event )          ||
			XCheckTypedEvent( d, MotionNotify, &event ) )
		{
	#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
			fprintf( stderr, "XEventManager::enterEventLoop: 1st priority message (preempted)\n" );
	#endif
		}
		else if ( XCheckTypedEvent( d, ClientMessage, &event ) )
		{
	#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
			fprintf( stderr, "XEventManager::enterEventLoop: 2nd priority message (preempted)\n" );
	#endif
		} else {
			XNextEvent( d, &event );
	#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
			switch ( event.type )
			{
			case ButtonPress:
			case ButtonRelease:
			case KeyPress:
			case KeyRelease:
			case ConfigureNotify:
			case Expose:
				fprintf( stderr, "XEventManager::enterEventLoop: 1st priority message\n" );
				break;
			case ClientMessage:
				fprintf( stderr, "XEventManager::enterEventLoop: 2nd priority message\n" );
				break;
			default:
				fprintf( stderr, "XEventManager::enterEventLoop: least priority message\n" );
			}
	#endif
		}
#else
		XNextEvent( d, &event );
#endif

		Event* e = null;

		//fprintf( stderr, "XEventManager::enterEventLoop: object count: %lli\n", Object::getObjectCount() );

			//  fprintf( stderr, "XEventManager::enterEventLoop(): %i (%i) - ", winId, event.xany.window ); 
#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
			printXEvent( &event, d );
#endif
			//  fprintf( stderr, "Event loop start: %i\n", Object::getObjectCount() );
		
		switch ( event.type )
		{
		case KeyPress:
			//if ( !winId || ( winId == event.xany.window ) )
			{
				this->countKeyPress++;
				e = this->processKeyPress( &event, 1 );
			}
			break;
		case KeyRelease:
			//if ( !winId || ( winId == event.xany.window ) )
			{
				this->countKeyPress++;
				e = this->processKeyPress( &event, 0 );
			}
			break;
		case ButtonPress:
			//if ( !winId || ( winId == event.xany.window ) )
			{
				this->countButtonPress++;
				e = this->processMousePress( &event );
			}
			break;
		case ButtonRelease:
			//if ( !winId || ( winId == event.xany.window ) )
			{
				this->countButtonRelease++;
				e = this->processMouseRelease( &event );
			}
			break;
		case MotionNotify:
			//if ( !winId || ( winId == event.xany.window ) )
			{
				while ( XCheckWindowEvent( d, event.xany.window, PointerMotionMask, &next ) )
				{
					event.xmotion = next.xmotion;
					//fprintf( stdout, "#" );
				}
				//fprintf( stdout, "\n" );
				this->countMotion++;
				e = this->processMotion( &event );
			}
			break;
//			{
//				while ( XCheckWindowEvent( d, event.xany.window, ExposureMask, &next ) )
//				{
//					event.xexpose = next.xexpose;
//				}
//				fprintf( stdout, "Expose\n" );
//				e = this->processExpose( (void*) &event );
//			}
//			break;
		case ConfigureNotify:
			//fprintf( stderr, "XEventManager::enterEventLoop: border %i\n", next.xconfigure.border_width );
			//fprintf( stderr, "ConfigureNotify %i:%i - %i:%i (%i) send(%i) event(%i) serial(%i)\n",
			//	event.xconfigure.x,
			//	event.xconfigure.y,
			//	event.xconfigure.width,
			//	event.xconfigure.height,
			//	event.xconfigure.border_width,
			//	event.xconfigure.send_event,
			//	event.xconfigure.event,
			//	event.xconfigure.serial
			//);
		
		case Expose:
			{
				bool send_reposition = false;
				bool send_resize     = false;
				bool send_exposure   = false;
				
				next = event;
				
				do {
					switch ( event.xany.type )
					{
					case ConfigureNotify:
					
						//	ConfigureNotifys are received in three different scenarios:
						//	1) Window has been repositioned
						//	2) Window has been resized
						//	3) Window needs to be updated
						//
						//	In scenario 1 the message is sent by the window manager, which
						//	is moving the window. The "send_event" field of the message is
						//	set to 1.
						//
						//	In scenario 2 the message is generated by the X Server. The
						//	"send_event" field of the the message is set to 0.
						//
						//	In scenario 3 the message is sent by the client program and
						//	therefore the "send_event" field is set to 1.
						//
						//	This makes it difficult to differentiate between messages sent
						//	from the client program and reposition messages sent from the
						//	window manager. In this case we assume that messages sent from
						//	the client program will set the "event" field as 0.
					
					
						if ( event.xconfigure.send_event && event.xconfigure.event )
						{
							send_reposition = true;
							reposition.xconfigure = next.xconfigure;
						//	fprintf( stdout, "Reposition\n" );
						} else {
							send_resize = true;
							
							//Window parent;
							//int x;
							//int y;
							//unsigned int width;
							//unsigned int height;
							//unsigned int border;
							//unsigned int depth;
							
							//fprintf( stderr, "XEventManager::enterEventLoop: window: %i\n", event.xany.window );
							//if ( 0 != event.xany.window )
							//{
							//	XGetGeometry( d, event.xany.window, &parent, &x, &y, &width, &height, &border, &depth );
							//	//fprintf( stderr, "XEventManager::enterEventLoop: XGetBorder: %i\n", border );
							//}
							
							resize.xconfigure = next.xconfigure;
						//	fprintf( stdout, "Resize\n" );
						}
					}
				}
				while ( 0 );//XCheckWindowEvent( d, event.xany.window, StructureNotifyMask, &next ) );

				if ( send_reposition )
				{
					//fprintf( stdout, "\tXEventManager: Reposition\n" );
					e = this->processReposition( (void*) &reposition );
					this->postEvent( e );
					e = null;
				}
				
				if ( send_resize )
				{
					//fprintf( stdout, "\tXEventManager: Resize\n" );
					e = this->processResize( (void*) &resize );
					this->postEvent( e );
					e = null;
					
					//	This forces as expose event to be sent to the Panel
					//	when a resize event occurs.
					//
					//	This is to make the system work well with Windows, which only allows
					//	you to paint when you have been sent an ExposeEvent. By having
					//	ExposeEvents generated with ResizeEvents here we are able to use
					//	a window system indenpendent Panel::dispatchEvent method.
					
					expose.xexpose = event.xexpose;
					e = this->processExpose( (void*) &expose );
					this->postEvent( e );
				}

				do {
					switch ( event.xany.type )
					{
					case Expose:
						expose.xexpose = event.xexpose;
						send_exposure = true;
						//fprintf( stdout, "Expose\n" );
						e = this->processExpose( (void*) &expose );
						//fprintf( stdout, "Expose\n" );
						this->postEvent( e );
						break;
					}
				}
				while ( 0 ); //XCheckWindowEvent( d, event.xany.window, ExposureMask, &next ) );




				if ( send_exposure )
				{
				}
				e = null;
			}
			break;
		case FocusIn:
		case FocusOut:
			this->countFocus++;
			e = this->processFocus( (void*) &event );
			break;
		case ClientMessage:
			{


#ifdef OPENOCL_UI_X11_XEVENTMANAGER_DEBUG
				fprintf( stderr, "XEventManager::ClientMessage\n" );
#endif
				if ( !winId || ( winId == event.xany.window ) )
				{
					const Atom atom          = XInternAtom( d, X_CLOSE, false );
					const Atom refresh_atom  = XInternAtom( d, OCL_REFRESH, 0 );
					const Atom relayout_atom = XInternAtom( d, OCL_RELAYOUT, 0 );
					const Atom redraw_atom   = XInternAtom( d, OCL_REDRAW, 0 );
					const Atom expose_atom   = XInternAtom( d, OCL_EXPOSE, 0 );

					XClientMessageEvent* cm = (XClientMessageEvent*) &event;
					const Atom event_atom = (Atom) cm->data.l[0];
					if ( event_atom == atom )
					{
#ifdef OPENOCL_UI_X11_XEVENTMANAGER_DEBUG
						fprintf( stderr, "closing %x\n", cm->data.l[0] );
						fprintf( stderr, "atom    %x\n", atom );
#endif						
						e = new CloseEvent( *this, cm->window );

						//fprintf( stderr, "clientmessage.message_type: %i\n", cm->message_type );
						//fprintf( stderr, "clientmessage.format:       %i\n", cm->format );
						//fprintf( stderr, "clientmessage.format:       %i\n", atom );
						
						//*visible = false;
					}
					else if ( event_atom == refresh_atom )
					{
						//fprintf( stderr, "XEventManager: ClientMessage: refresh\n" );
						e = new RefreshEvent( *this, cm->window );
					}
					else if ( event_atom == relayout_atom )
					{
						//fprintf( stderr, "XEventManager: ClientMessage: relayout\n" );
						e = new RelayoutEvent( *this, cm->window, 0, 0, 0 );
					}
					else if ( event_atom == redraw_atom )
					{
						//fprintf( stderr, "XEventManager: ClientMessage: redraw\n" );
						e = new RedrawEvent( *this, cm->window, 0, 0, 0 );
					}
					else if ( event_atom == expose_atom )
					{
						//fprintf( stderr, "XEventManager: ClientMessage: expose\n" );
						e = new ExposeEvent( *this, cm->window, 0, 0, 0, 0, 0 );
					}
					else
					{
						//fprintf( stderr, "XEventManager: ClientMessage: unknown\n" );
					}
				}
				//e = this->processClientMessage( (void*) &event, visible );
			}
			break;
		//case DestroyNotify:
		//	fprintf( stderr, "XEventManager::DestroyNotify\n" );
		//	{
		//		//XDestroyWindowEvent* destory_event = (XDestroyWindowEvent*) &event;
		//		//e = new CloseEvent( *WinEventManager::eventManager, (unsigned long) destroy_event->window );
		//	}
		//	break;
		}

		//fprintf( stderr, "-------------------------------\n" );
		//fprintf( stderr, "keypress: %i\n", this->countKeyPress );
		//fprintf( stderr, "button press: %i\n", this->countButtonPress );
		//fprintf( stderr, "button release: %i\n", this->countButtonRelease );
		//fprintf( stderr, "motion: %i\n", this->countMotion );
		//fprintf( stderr, "expose: %i\n", this->countExpose );
		//fprintf( stderr, "configure: %i\n", this->countConfigure );
		//fprintf( stderr, "focus: %i\n", this->countFocus );

		if ( null != e )
		{
			//fprintf( stderr, "XEventManager: type: %u\n", e->getType() ); 
			this->postEvent( e );
		}

	//    IO::out().println( "EventManager: posted event" );
	}

//    case ButtonPress:
//      this->fireEvent( new MouseEvent( (Object&) *this ) );
//      break;
//    case KeyPress:
//      this->fireEvent( new KeyEvent( (Object&) *this ) );
//      break;
//    default:
//      break;
//    }
//  }
}

static Time button1LastClickTime = 0; 

Event*
XEventManager::processMousePress( void* anEvent )
{
	XButtonEvent* event = (XButtonEvent*) anEvent;

	unsigned long int targetId = event->window;
	unsigned int mouseX = event->x;
	unsigned int mouseY = event->y;
	unsigned int buttonClickCount = 1;

	flags buttonClicked = 0;
	flags buttonState = 0;
	flags modifiers = 0;

	//	Determine if double click.

	Time current = event->time;
	if ( 500 > (current - button1LastClickTime) )
	{
		//fprintf( stderr, "XEventManager::processMousePress: double click\n" );
		buttonClickCount = 2;
	}
	button1LastClickTime = current;
	
	String button( event->button );
	Debug::println( "event->button" );
	Debug::println( button );

	if ( Button1Mask == (event->state & Button1Mask) )
	{
		buttonState |= MouseEvent::BUTTON1_PRESS;
	}
	if ( Button2Mask == (event->state & Button2Mask) )
	{
		buttonState |= MouseEvent::BUTTON2_PRESS;
	}
	if ( Button3Mask == (event->state & Button3Mask) )
	{
		buttonState |= MouseEvent::BUTTON3_PRESS;
	}
	if ( Button4Mask == (event->state & Button4Mask) )
	{
		buttonState |= MouseEvent::BUTTON4_PRESS;
	}
	if ( Button5Mask == (event->state & Button5Mask) )
	{
		buttonState |= MouseEvent::BUTTON5_PRESS;
	}

	if ( ShiftMask == (event->state & ShiftMask) )
	{
		modifiers |= InputEvent::SHIFT;
	}
	if ( ControlMask == (event->state & ControlMask) )
	{
		modifiers |= InputEvent::CTRL;
	}

	switch ( event->button )
	{
	case Button1:
		buttonClicked |= MouseEvent::BUTTON1_PRESS;
		break;
	case Button2:
		buttonClicked |= MouseEvent::BUTTON2_PRESS;
		break;
	case Button3:
		buttonClicked |= MouseEvent::BUTTON3_PRESS;
		break;
	case Button4:
		buttonClicked |= MouseEvent::BUTTON4_PRESS;
		break;
	case Button5:
		buttonClicked |= MouseEvent::BUTTON5_PRESS;
		break;
	}

	MouseEvent* mouse_event = new MouseEvent( *this, targetId, mouseX, mouseY, modifiers, buttonState, buttonClicked, buttonClickCount );

	return mouse_event;
}

Event*
XEventManager::processMouseRelease( void* anEvent )
{
  XButtonEvent* event = (XButtonEvent*) anEvent;

  unsigned long int targetId = event->window;
  unsigned int mouseX = event->x;
  unsigned int mouseY = event->y;
  unsigned int buttonClickCount = 1;
  flags buttonClicked = 0;
  flags buttonState = 0;
  flags modifiers = 0;

  //Debug::println( "event->button" );
  //Debug::println( new String( event->button ) );

  if ( Button1Mask == (event->state & Button1Mask) )
  {
    buttonState |= MouseEvent::BUTTON1_RELEASE;
  }
  if ( Button2Mask == (event->state & Button2Mask) )
  {
    buttonState |= MouseEvent::BUTTON2_RELEASE;
  }
  if ( Button3Mask == (event->state & Button3Mask) )
  {
    buttonState |= MouseEvent::BUTTON3_RELEASE;
  }
  if ( Button4Mask == (event->state & Button4Mask) )
  {
    buttonState |= MouseEvent::BUTTON4_RELEASE;
  }
  if ( Button5Mask == (event->state & Button5Mask) )
  {
    buttonState |= MouseEvent::BUTTON5_RELEASE;
  }

  switch ( event->button )
  {
  case Button1:
    buttonClicked = MouseEvent::BUTTON1_RELEASE;
    break;
  case Button2:
    buttonClicked = MouseEvent::BUTTON2_RELEASE;
    break;
  case Button3:
    buttonClicked = MouseEvent::BUTTON3_RELEASE;
    break;
  }

  return new MouseEvent( *this,
                         targetId,
                         mouseX,
                         mouseY,
			 modifiers,
                         buttonState,
                         buttonClicked,
                         buttonClickCount );
}

Event*
XEventManager::processKeyPress( void* anEvent, int press )
{
	XKeyEvent* event = (XKeyEvent*) anEvent;
	char* buffer = (char*) Runtime::calloc( 2, sizeof( char ) );
	KeySym keysym;
	XLookupString( event, buffer, 2, &keysym, NULL );

#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER_PROCESSKEYEVENT
	fprintf( stderr, "XEventManager::processKeyEvent: key_value: %i\n", *buffer );
	fprintf( stderr, "XEventManager::processKeyEvent: keysym: %i\n", keysym );
	fprintf( stderr, "XEventManager::processKeyEvent: state: %i (%i)(%i)\n", event->state );
#endif

	//	If an ordinary key is pressed it is stored in key_value and in keysym.
	//
	//	If a special key is pressed it is stored in keysym but not in the key_value,
	//	the X constants XK_Shift_L, XK_Control_L, etc...
	//	are used to investigate what the value of the keysym means.
	//
	//	If a key is acting as a modifier the state is changed.
	//
	//	0x0001 (1)	Shift
	//	0x0004 (4)  Ctrl
	//	0x0010 (16) Apple

	unsigned int key_value = *buffer;
	unsigned long modifiers = 0;

	KeyEvent* ke = null;

	if ( XEventManager::SHIFT == (XEventManager::SHIFT & event->state ) )
	{
		modifiers |= InputEvent::SHIFT;
	}
	if ( XEventManager::CTRL == (XEventManager::CTRL & event->state ) )
	{
		modifiers |= InputEvent::CTRL;
	}
	if ( XEventManager::APPLE == (XEventManager::APPLE & event->state ) )
	{
		modifiers |= InputEvent::SPECIAL;
	}
	
	switch ( *buffer )
	{
	case 0:
		switch ( keysym )
		{
		case XK_Left:
			key_value = KeyEvent::LEFT;
			break;
		case XK_Right:
			key_value = KeyEvent::RIGHT;
			break;
		case XK_Up:
			key_value = KeyEvent::UP;
			break;
		case XK_Down:
			key_value = KeyEvent::DOWN;
			break;
		case XK_Shift_L:
			key_value = KeyEvent::SHIFT_LEFT;
			break;
		case XK_Shift_R:
			key_value = KeyEvent::SHIFT_RIGHT;
			break;
		case XK_Control_L:
			key_value = KeyEvent::CTRL_LEFT;
			break;
		case XK_Control_R:
			key_value = KeyEvent::CTRL_RIGHT;
			break;
		case XK_ISO_LEFT_TAB:
			key_value = KeyEvent::TAB;
		}
	default:
		switch ( press )
		{
		case 0:
			ke = new KeyUpEvent( *this, event->window, event->x, event->y, modifiers, key_value );
			break;
		case 1:
			ke = new KeyDownEvent( *this, event->window, event->x, event->y, modifiers, key_value );
			break;
		}
	}
	Runtime::free( buffer );
	return ke;
}

Event*
XEventManager::processExpose( void* anEvent )
{
  XExposeEvent* event = (XExposeEvent*) anEvent;
  
  if ( event->send_event )
  {
#ifdef OPENOCL_UI_X11_XEVENTMANAGER_DEBUG
  	fprintf( stderr, "XEventManager: send_event expose event\n" );
#endif
  }
  return new ExposeEvent( (Object&) *this,
                          event->window,
                          event->x,
                          event->y,
                          event->width,
                          event->height,
                          event->count );
}

	//
	//	Related post http://lists.gnu.org/archive/html/bug-gnustep/2004-02/msg00146.html
	//
	//	Synthetic (send_event = true) events are sent when the window is repositioned.
	//	Non-synthetic (send_event = false) events are sent when the window is resized.
	//
	//	For some unknown reason the x:y position of the resize events is not relative to
	//	its parent window. Here we use this to differentiate between Reposition and Resize
	//	events and propagate each accordingly.
	//
	//	However, this causes confusion with real synthetic events created by OpenOCL UI
	//	providers (such as this). Here we also test event->event. It seems that this is
	//	passed unmodified from SendEvent and if sent as 0 can be checked against for
	//	a real synthetic event.

Event*
XEventManager::processReposition( void* anEvent )
{
	XConfigureEvent* event = (XConfigureEvent*) anEvent;
	
#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
	fprintf( stderr, "\tXEventManager::processReposition: %i:%i\n", event->x, event->y );
#endif
	
	return new RepositionEvent( (Object&) *this, event->window, event->x, event->y, event->above );
}

Event*
XEventManager::processResize( void* anEvent )
{
	XConfigureEvent* event = (XConfigureEvent*) anEvent;


	//	Am I the only one on crazy pills? Does border width mean something that I don't know
	//	about. Anyway I'm getting crazy values for border_width so I'm just setting this as
	//	0.

#ifdef DEBUG_OPENOCL_UI_X11_XEVENTMANAGER
	fprintf( stderr, "\tXEventManager::processResize: %i:%i\n", event->width, event->height );
#endif
	//fprintf( stderr, "XEventManager::processResize: xevent->border_width: %i\n", event->border_width );

	return new ResizeEvent( (Object&) *this, event->window, event->width, event->height, 0 );
}

Event*
XEventManager::processMotion( void* anEvent )
{
	XMotionEvent* event = (XMotionEvent*) anEvent;
	flags buttonState = 0;
	flags modifiers = 0;

	if ( Button1Mask == (event->state & Button1Mask) )
	{
		buttonState |= PointerEvent::BUTTON1;
	}
	if ( Button2Mask == (event->state & Button2Mask) )
	{
		buttonState |= PointerEvent::BUTTON2;
	}
	if ( Button3Mask == (event->state & Button3Mask) )
	{
		buttonState |= PointerEvent::BUTTON3;
	}

	//	XXX What about modifiers??

	return new MotionEvent( (Object&) *this,
                                event->window,
                                event->x,
                                event->y,
			        modifiers,
                                buttonState );
}

Event*
XEventManager::processFocus( void* anEvent )
{
	XFocusChangeEvent* event = (XFocusChangeEvent*) anEvent;
	int event_type;
	switch ( event->type )
	{
	case FocusIn:
		event_type = FocusEvent::TAKE_FOCUS;
		break;
	case FocusOut:
		event_type = FocusEvent::LOSE_FOCUS;
		break;
	};
	return new FocusEvent( (Object&) *this, event->window, event_type );
}

Event*
XEventManager::processClientMessage( void* anEvent, bool* visible )
{
	XClientMessageEvent* event = (XClientMessageEvent*) anEvent;
	Event* e = new ActionEvent( (Object&) *this, event->window, "openocl::ui::EventManager", "Close" );
	*visible = false;
	return e;
}

void
XEventManager::SendCustomEvent( void* display, long winId, mask eventType )
{
	Display* dpy    = (Display*) display;
	Window   win_id = (Window) winId;
	long event_mask = 0;

	XClientMessageEvent event;
	event.type = ClientMessage;
	event.serial = 0;
	event.send_event = 1;
	event.display = dpy;
	event.window = win_id;
	event.message_type = 220;
	event.format = 32;

	switch ( eventType )
	{
	case Event::REFRESH_EVENT:
		//fprintf( stderr, "--> XEventManager::SendCustomEvent: RELAYOUT\n" );
		event.data.l[0] = XInternAtom( dpy, "OPENOCL_UI_X11_XEVENTMANAGER_REFRESH", 0 );
		break;
	case Event::RELAYOUT_EVENT:
		//fprintf( stderr, "--> XEventManager::SendCustomEvent: RELAYOUT\n" );
		event.data.l[0] = XInternAtom( dpy, "OPENOCL_UI_X11_XEVENTMANAGER_RELAYOUT", 0 );
		break;
	case Event::REDRAW_EVENT:
		//fprintf( stderr, "--> XEventManager::SendCustomEvent: REDRAW\n" );
		event.data.l[0] = XInternAtom( dpy, "OPENOCL_UI_X11_XEVENTMANAGER_REDRAW", 0 );
		break;
	case Event::EXPOSE_EVENT:
		//fprintf( stderr, "--> XEventManager::SendCustomEvent: EXPOSE\n" );
		event.data.l[0] = XInternAtom( dpy, "OPENOCL_UI_X11_XEVENTMANAGER_EXPOSE", 0 );
		break;
	default:
		return;
	}

	XSendEvent( dpy, win_id, 1, event_mask, (XEvent*) &event );
	XFlush( dpy );
}

void
XEventManager::SendEvent( void* display, long winId, bool propagate, long eventMask, void* xevent )
{
	Display* dpy = XOpenDisplay( null );
	Window win_id = (Window) winId;
	XEvent* event = (XEvent*) xevent;

	XSendEvent( dpy, win_id, propagate, eventMask, event );
	XCloseDisplay( dpy );
}




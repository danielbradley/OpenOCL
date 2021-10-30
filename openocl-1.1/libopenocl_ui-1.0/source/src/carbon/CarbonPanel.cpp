/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <openocl/ui/carbon/CarbonDisplay.h>
#include <openocl/ui/carbon/CarbonEventManager.h>
#include <openocl/ui/carbon/CarbonGraphicsContext.h>
#include <openocl/ui/carbon/CarbonPanel.h>
#include <openocl/ui/carbon/CarbonVisual.h>
#include <openocl/ui/Geometry.h>

#include <openocl/base/Runtime.h>
#include <openocl/base/event/ConfigureEvent.h>
#include <openocl/base/event/ExposeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventSource.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>

#include <Carbon/Carbon.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::ui::carbon;

void finderror( OSStatus error_code );
OSStatus PanelMouseEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData );
OSStatus PanelControlEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData );
OSStatus PanelWindowEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData );

/*
 *
	kWindowNoAttributes
	kWindowCloseBoxAttribute
   kWindowHorizontalZoomAttribute = (1L << 1),
   kWindowVerticalZoomAttribute = (1L << 2),
   kWindowFullZoomAttribute = (kWindowVerticalZoomAttribute | kWindowHorizontalZoomAttribute),
   kWindowCollapseBoxAttribute = (1L << 3),
   kWindowResizableAttribute = (1L << 4),
   kWindowSideTitlebarAttribute = (1L << 5),
   kWindowToolbarButtonAttribute = (1L << 6),
   kWindowMetalAttribute = (1L << 8),
   kWindowNoUpdatesAttribute = (1L << 16),
   kWindowNoActivatesAttribute = (1L << 17),
   kWindowOpaqueForEventsAttribute = (1L << 18),
   kWindowCompositingAttribute = (1L << 19),
   kWindowNoShadowAttribute = (1L << 21),
   kWindowHideOnSuspendAttribute = (1L << 24),
   kWindowStandardHandlerAttribute = (1L << 25),
   kWindowHideOnFullScreenAttribute = (1L << 26),
   kWindowInWindowMenuAttribute = (1L << 27),
   kWindowLiveResizeAttribute = (1L << 28),
   kWindowIgnoreClicksAttribute = (1L << 29),
   kWindowNoConstrainAttribute = (1L << 31),
   kWindowStandardDocumentAttributes = (kWindowCloseBoxAttribute
| kWindowFullZoomAttribute | kWindowCollapseBoxAttribute | kWindowResizableAttribute),
   kWindowStandardFloatingAttributes = (kWindowCloseBoxAttribute
| kWindowCollapseBoxAttribute)
};
*/
CarbonPanel::CarbonPanel( const CarbonVisual& aVisual, flags type )
{
	this->geometry = new Geometry();
	this->visual = &aVisual;
	this->winId = null;

	OSStatus status;
		
	WindowClass win_class = 0;// kAltPlainWindowClass;
	WindowAttributes attributes = kWindowNoAttributes;
	Rect bounds;

	switch ( type )
	{
	case Panel::APPLICATION:
		win_class = kDocumentWindowClass;
	
		attributes = kWindowNoAttributes;
		attributes |= kWindowStandardDocumentAttributes;
		attributes |= kWindowResizableAttribute;
		attributes |= kWindowLiveResizeAttribute;
		attributes |= kWindowToolbarButtonAttribute;
		attributes |= kWindowNoConstrainAttribute;
		attributes |= kWindowStandardHandlerAttribute;
		attributes |= kWindowCompositingAttribute;
		
		
		//attributes |= kWindowSideTitlebarAttribute;
		//		attributes |= kWindowCloseBoxAttribute;
		break;
	default:
		;
	}
		
	bounds.top = 100;
	bounds.left = 100;
	bounds.bottom = 800;
	bounds.right = 800;
	
	if ( (status = CreateNewWindow( win_class, attributes, &bounds, (WindowRef*) &this->winId )) )
	{
		finderror( status );
		fprintf( stderr, "CarbonPanel::CarbonPanel(): could not create window\n" );
	}

	//
	//	Install Window Event Handler.

	EventTypeSpec* mouse_events = (EventTypeSpec*) Runtime::calloc( 4, sizeof( EventTypeSpec ) );
	mouse_events[0].eventClass = kEventClassMouse;
	mouse_events[0].eventKind  = kEventMouseMoved;
	mouse_events[1].eventClass = kEventClassMouse;
	mouse_events[1].eventKind  = kEventMouseDown;
	mouse_events[2].eventClass = kEventClassMouse;
	mouse_events[2].eventKind  = kEventMouseUp;
	mouse_events[3].eventClass = kEventClassMouse;
	mouse_events[3].eventKind  = kEventMouseDragged;
	InstallWindowEventHandler( (WindowRef) this->winId, PanelMouseEventHandler, 4, mouse_events, (void*) this, NULL);

	static const EventTypeSpec window_events = { kEventClassWindow, kEventWindowBoundsChanged };
	InstallWindowEventHandler( (WindowRef) this->winId, PanelWindowEventHandler, 1, &window_events, (void*) this, NULL);
	
	//	XXX
	//	This is an extremely kludgy way of finding the content view as it
	//	assumes that the content view will always be the last control.

	static const EventTypeSpec event_types = { kEventClassControl, kEventControlDraw };
	
	HIViewRef rootview_ref = HIViewGetRoot( (WindowRef) this->winId );
	HIViewRef subview = HIViewGetFirstSubview( rootview_ref );
	HIViewRef last_view = null;
	if ( subview )
	{
		do
		{
			last_view = subview;
		} while ( (subview = HIViewGetNextView( subview )) );
		InstallControlEventHandler ( last_view, PanelControlEventHandler, 1, &event_types, (void*) this, NULL);
	}
	this->contentId = (void*) last_view;

	EventSource& event_source = this->visual->getCarbonDisplay().getEventSource();
	event_source.registerDispatcher( (unsigned long) this->winId, *this );

	//XEventManager& em = this->xvisual->getXDisplay().getXEventManager();
	//em.registerDispatcher( this->winId, *this );


	this->gc = new CarbonGraphicsContext( *this );
}

CarbonPanel::~CarbonPanel()
{
	DisposeWindow( (WindowRef) this->winId );
}

//	public virtual methods (EventDispatcher)
void
CarbonPanel::dispatchEvent( openocl::base::event::Event* anEvent )
{
	flags type = anEvent->getType();

	switch ( Event::WINDOW_EVENT & type )
	{
	case Event::WINDOW_EVENT:
		switch ( type )
		{
		case Event::CONFIGURE_EVENT:
			{
				fprintf( stderr, "CarbonPanel::dispatchEvent( CONFIGURE_EVENT )\n" );
				ConfigureEvent* ce = dynamic_cast<ConfigureEvent*>( anEvent );
				//int x = ce->getX();
				//int y = ce->getY();
				int w = ce->getWidth();
				int h = ce->getHeight();
				
				this->setBoundsAndGeometry( 0, 1, w, h );	// prototype differs!
				this->doLayout( *this->gc );
				this->status = Panel::REDRAW;
				//this->paint( *this->gc )
			}
			break;
		case Event::EXPOSE_EVENT:
			fprintf( stderr, "CarbonPanel::dispatchEvent( EXPOSE_EVENT )\n" );
			this->doLayout( *this->gc );
			this->redraw( *this->gc );
			break;
		}
		break;
	default:
		switch( Event::POINTER_EVENT & type )
		{
		case Event::POINTER_EVENT:
			PointerEvent* pe = dynamic_cast<PointerEvent*>( anEvent );
			if ( pe )
			{
				int panel_x = this->getAbsoluteX();
				int panel_y = this->getAbsoluteY();
				int x = pe->getX();
				int y = pe->getY();
				int event_absolute_x = panel_x + x;
				int event_absolute_y = panel_y + y;
				pe->setAbsoluteCoordinates( event_absolute_x, event_absolute_y );
			}
		}
		this->Container::forwardEvent( *anEvent );
	}
	this->update();
	this->flush();
	delete anEvent;
}


	//	public virtual methods (Component)
void CarbonPanel::add( Component* aComponent )
{
	aComponent->setContainingPanel( *this );
	this->Container::add( aComponent );
	this->relayout();
}

void CarbonPanel::add( Component& aComponent )
{
	aComponent.setContainingPanel( *this );
	this->Container::add( aComponent );
	this->relayout();
}

void CarbonPanel::processEvent( const openocl::base::event::Event& anEvent ) {}

void
CarbonPanel::setBounds( int x, int y, unsigned int width, unsigned int height )
{
	//this->Container::setBounds( x, y, width, height );

	//	XXX
	//	Need to request size for this panel.
}

void CarbonPanel::setName( const openocl::base::String& aName ) {}

	//	public virtual methods (Panel)

Panel*
CarbonPanel::createChildPanel( flags type ) const
{
	return null;
}

Panel*
CarbonPanel::createSubPanel() const
{
	return null;
}

void CarbonPanel::clear() {}

void
CarbonPanel::requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border )
{
	WindowRef window_id = (WindowRef) this->winId;
	MoveWindow( window_id, x, y, false );
	SizeWindow( window_id, width, height, true ); 
}

void
CarbonPanel::requestPosition( int x, int y )
{
	WindowRef window_id = (WindowRef) this->winId;
	MoveWindow( window_id, x, y, false );
}

void
CarbonPanel::requestSize( unsigned int width, unsigned int height )
{
	WindowRef window_id = (WindowRef) this->winId;
	SizeWindow( window_id, width, height, true ); 
}


void CarbonPanel::resetCursor() {}
void CarbonPanel::setCursor( unsigned int shape ) {}
void CarbonPanel::setCursor( const Image& anImage, const Image& aMask, int hotspotX, int hotspotY ) {}

void
CarbonPanel::map()
{
	OSStatus status;
	SelectWindow( (WindowRef) this->winId );
	ShowWindow( (WindowRef) this->winId );

	if ( (status = TransitionWindow( (WindowRef) this->winId, kWindowSlideTransitionEffect, kWindowShowTransitionAction, null )) )
	{
		fprintf( stderr, "CarbonPanel::CarbonPanel(): could not show transition window\n" );
	}
	
	//if ( IsWindowActive( (WindowRef) this->winId ) )
	//{
	//	fprintf( stderr, "Window active\n" );
	//} else {
	//	fprintf( stderr, "Window inactive\n" );
	//}
}

void
CarbonPanel::unmap()
{
	OSStatus status;
	if ( (status = TransitionWindow( (WindowRef) this->winId, kWindowSlideTransitionEffect, kWindowHideTransitionAction, null )) )
	{
		fprintf( stderr, "CarbonPanel::CarbonPanel(): could not hide transition window\n" );
	}
}

void
CarbonPanel::show( int type )
{}

void CarbonPanel::update()
{
	//	This needs to send events using Carbon Event
	//  method.

	HIViewSetNeedsDisplay( (HIViewRef) this->contentId, true );

	//this->doLayout( *this->gc );
	//this->repaint();
}

void
CarbonPanel::flush() const
{}

//	public virtual constant methods (Component)
int
CarbonPanel::getAbsoluteX() const
{
	int absolute_x = 0;
	try
	{
		absolute_x += this->getContainingPanel().getAbsoluteX();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_x += this->geometry->x;
	
	return absolute_x;
}

int
CarbonPanel::getAbsoluteY() const
{
	int absolute_y = 0;
	try
	{
		absolute_y = this->getContainingPanel().getAbsoluteY();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_y += this->geometry->y;
	
	return absolute_y;
}

bool
CarbonPanel::draw( GraphicsContext& aGraphics ) const
{
	return false;
}

	//	public  constant methods (Drawable)
unsigned long int
CarbonPanel::getId() const
{
	return (unsigned long) this->winId;
}

unsigned int
CarbonPanel::getScreen() const
{
	return 0;
}

	//	public virtual constant methods (EventDispatcher)
long unsigned int
CarbonPanel::getEventDispatcherId() const
{
	return 0;
}

//---------------------------------------------------------------------------------
//	public virtual constant methods (Panel)
//---------------------------------------------------------------------------------
	
void CarbonPanel::setName( const openocl::base::String& aName ) const
{
	const_cast<CarbonPanel*>( this )->setName( aName );
}

const Visual&
CarbonPanel::getVisual() const
{
	return *this->visual;
}

GraphicsContext&
CarbonPanel::getGraphicsContext() const
{
	return *this->gc;
}

unsigned int
CarbonPanel::getDrawableWidth() const
{
	return 0;
}

unsigned int
CarbonPanel::getDrawableHeight() const
{
	return 0;
}

unsigned int
CarbonPanel::getDrawableDepth() const
{
	return 0;
}

bool
CarbonPanel::redraw( GraphicsContext& gc )
{
	bool draw_again = false;
	
	//draw_again |= this->Component::draw( gc );
	draw_again |= this->Container::draw( gc );
	
	//gc.setForeground( Color::BLACK );
	//gc.setBackground( Color::RED );

	//gc.drawFilledRectangle( 10, 10, 40, 20 );

	return draw_again;
}

void
CarbonPanel::setBoundsAndGeometry( int x, int y, unsigned int width, unsigned int height )
{
	this->Container::setBounds( x, y, width, height );
	
	//	XXX
	//	Need to set geometry as well.

	//this->update();

}


//  virtual const AbstractDisplay& getAbstractDisplay() const = 0 {}
//  virtual unsigned int getScreen() const = 0 {}

void finderror( OSStatus error_code )
{
	fprintf( stderr, "error: %i\n", error_code );

	switch ( error_code )
	{
	case noErr:
		fprintf( stderr, "No error\n" );
		break;
	case paramErr:
		fprintf( stderr, "Invalid value passed in a parameter\n" );
		break;
	case memFullErr:
		fprintf( stderr, "Memory full\n" );
		break;
	case resNotFound:
		fprintf( stderr, "Resource not found\n" );
		break;
	case errInvalidWindowPtr:
		fprintf( stderr, "Invalid window pointer\n" );
		break;
	case errUnsupportedWindowAttributesForClass:
		fprintf( stderr, "Attribute bits are inappropriate for the specified window class\n" );
		break;
	case errWindowDoesNotHaveProxy:
		fprintf( stderr, "No proxy attached to window\n" );
		break;
	case errInvalidWindowProperty:
		fprintf( stderr, "Invalid window property\n" );
		break;
	case errWindowPropertyNotFound:
		fprintf( stderr, "Window property not found\n" );
		break;
	case errUnrecognizedWindowClass:
		fprintf( stderr, "Unrecognized window class\n" );
		break;
	case errCorruptWindowDescription:
		fprintf( stderr, "Corrupt window description\n" );
		break;
	case errUserWantsToDragWindow:
		fprintf( stderr, "User wants to drag window\n" );
		break;
	case errWindowsAlreadyInitialized:
		fprintf( stderr, "Window's Already initialized\n" );
		break;
	case errFloatingWindowsNotInitialized:
		fprintf( stderr, "Floating window's not initialized\n" );
		break;
	default:
		fprintf( stderr, "Unknown error\n" );
		break;
	}
}

//void
//CarbonPanel::redraw()

OSStatus PanelMouseEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData )
{
	int event_class = GetEventClass( theEvent );
	int event_kind  = GetEventKind( theEvent );
	
	WindowRef target_id;
	GetEventParameter( theEvent, kEventParamWindowRef, typeWindowRef, NULL, sizeof(WindowRef), NULL, &target_id );

	fprintf( stderr, "PanelMouseEventHandler( target( %x ) userData( %x ) class(%x) kind(%x) )\n", target_id, userData, event_class, event_kind );
	CarbonPanel* panel = reinterpret_cast<CarbonPanel*>( userData );
	if ( panel )
	{
		switch ( event_kind )
		{
		case kEventMouseUp:
			fprintf( stderr, "PanelMouseEventHandler( MOUSE_UP )\n" );
			break;
		case kEventMouseDown:
			fprintf( stderr, "PanelMouseEventHandler( MOUSE_DOWN )\n" );
			break;
		case kEventMouseDragged:
			fprintf( stderr, "PanelMouseEventHandler( MOUSE_DRAGGED )\n" );
			break;
		case kEventMouseMoved:
			fprintf( stderr, "PanelMouseEventHandler( MOUSE_MOVED )\n" );
			break;
		default:
			fprintf( stderr, "PanelMouseEventHandler( UNKNOWN: %i )\n", event_kind );
		}

		unsigned int button;
		unsigned long modifiers;
		Point point;

		switch ( event_kind )
		{
		case kEventMouseUp:
		case kEventMouseDown:
		case kEventMouseDragged:
			{
				GetEventParameter( theEvent, kEventParamMouseButton, typeMouseButton, NULL, sizeof(unsigned int), NULL, &button );
			}
			//	Intentional fall through
		case kEventMouseMoved:
			{
				GetEventParameter( theEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &point );
				GetEventParameter( theEvent, kEventParamKeyModifiers, typeUInt32, NULL, sizeof(unsigned long), NULL, &modifiers );
			}
			break;
		}

		Event* event = null;
		unsigned long target_id = (unsigned long) panel;
		int x = 0;
		int y = 0;
		int button_click_count = 0;
		int button_clicked = 0;
		flags button_state = 0;
		flags key_modifiers = 0;

		switch ( event_kind )
		{
		case kEventMouseDown:
			if ( kEventMouseButtonPrimary == (kEventMouseButtonPrimary & button ))
			{
				button_clicked = MouseEvent::BUTTON1_PRESS;
			}
			else if ( kEventMouseButtonSecondary == (kEventMouseButtonSecondary & button ))
			{
				button_clicked = MouseEvent::BUTTON2_PRESS;
			}
			else if ( kEventMouseButtonTertiary == (kEventMouseButtonTertiary & button ))
			{
				button_clicked = MouseEvent::BUTTON3_PRESS;
			}
			break;
		case kEventMouseDragged:
			if ( kEventMouseButtonPrimary == (kEventMouseButtonPrimary & button ))
			{
				button_state |= PointerEvent::BUTTON1;
			}
			if ( kEventMouseButtonSecondary == (kEventMouseButtonSecondary & button ))
			{
				button_state |= PointerEvent::BUTTON2;
			}
			else if ( kEventMouseButtonTertiary == (kEventMouseButtonTertiary & button ))
			{
				button_state |= PointerEvent::BUTTON3;
			}
			break;
		case kEventMouseUp:
			if ( kEventMouseButtonPrimary == (kEventMouseButtonPrimary & button ))
			{
				button_clicked = MouseEvent::BUTTON1_RELEASE;
			}
			else if ( kEventMouseButtonSecondary == (kEventMouseButtonSecondary & button ))
			{
				button_clicked = MouseEvent::BUTTON2_RELEASE;
			}
			else if ( kEventMouseButtonTertiary == (kEventMouseButtonTertiary & button ))
			{
				button_clicked = MouseEvent::BUTTON3_RELEASE;
			}
			break;
		}

		switch ( event_kind )
		{
		case kEventMouseUp:
		case kEventMouseDown:
			event = new MouseEvent( *panel, target_id, x, y, button_click_count, button_clicked, button_state, key_modifiers );
			break;
		case kEventMouseDragged:
		case kEventMouseMoved:
			event = new MotionEvent( *panel, target_id, x, y, button_state, key_modifiers );
			break;
		}
		
		panel->dispatchEvent( event );
	}

	EventRef event_copy = CopyEvent( theEvent );
	OSStatus err = CallNextEventHandler( nextHandler, event_copy );
	return err;
}
	
OSStatus PanelControlEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData )
{
	fprintf( stderr, "PanelControlEventHander()\n" );
	OSStatus err = CallNextEventHandler( nextHandler, theEvent );
	CarbonPanel* panel = reinterpret_cast<CarbonPanel*>( userData );
	if ( panel )
	{
		int event_kind = GetEventKind( theEvent );
		fprintf( stderr, "\tControl event: " );
		switch ( event_kind )
		{
		case kEventControlDraw:
			{
				CarbonGraphicsContext& gc = dynamic_cast<CarbonGraphicsContext&>( panel->getGraphicsContext() );
				const Object& source = *CarbonEventManager::eventManager;
				unsigned long target_id = (unsigned long) userData;	
				gc.beginPaint( theEvent );
				{
					ExposeEvent* event = new ExposeEvent( source, target_id, 0, 0, 0, 0, 0 );
					panel->dispatchEvent( event );
				}
				gc.endPaint( theEvent );
			}
			break;
		default:
			fprintf( stderr, "unhandled control event (%i)\n", event_kind );
		}
	}
	return err;
}

OSStatus PanelWindowEventHandler( EventHandlerCallRef nextHandler, EventRef theEvent, void* userData )
{
	OSStatus err = CallNextEventHandler( nextHandler, theEvent );
	CarbonPanel* panel = reinterpret_cast<CarbonPanel*>( userData );
	if ( panel )
	{
		int event_kind = GetEventKind( theEvent );
		switch ( event_kind )
		{
		case kEventWindowBoundsChanged:
			{
				const Object& source = *CarbonEventManager::eventManager;
				unsigned long target_id = (unsigned long) userData;
				int x, y, w, h, b;
				unsigned long wb = 0;
				
				Rect bounds;
				GetEventParameter( theEvent, kEventParamCurrentBounds, typeQDRectangle, NULL, sizeof(Rect), NULL, &bounds );
				x = bounds.left;
				y = bounds.top;
				w = bounds.right - bounds.left;
				h = bounds.bottom - bounds.top;
				b = 0;
				
				ConfigureEvent* event = new ConfigureEvent( source, target_id, x, y, w, h, b, wb );
				
				panel->dispatchEvent( event );
			}
			break;
		case kEventWindowShown:
			fprintf( stderr, "shown\n" );
			{
				panel->relayout();
				panel->update();
			}
			break;
		case kEventWindowBoundsChanging:
		default:
			fprintf( stderr, "unhandled window event (%i)\n", event_kind );
		}
	}
	return err;
}


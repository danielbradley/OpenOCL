/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <windows.h>
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/win32/WindowsUndefs.h"
#include "openocl/ui/win32/WinGraphicsContext.h"
#include "openocl/ui/win32/WinEventManager.h"
#include "openocl/ui/win32/WinPanel.h"
#include "openocl/ui/win32/WinPixmap.h"
#include "openocl/ui/win32/WinScreen.h"
#include "openocl/ui/win32/WinVisual.h"
#include <openocl/base/FormattedString.h>
#include <openocl/base/String.h>
#include <openocl/base/RuntimeException.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ConfigureEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventSource.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/base/event/RepositionEvent.h>
#include <openocl/base/event/ResizeEvent.h>
#include <openocl/imaging/Color.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::ui::win32;

#ifndef WS_EX_COMPOSITED
#define WS_EX_COMPOSITED 0x02000000L
#endif

#ifndef WS_EX_NOACTIVATE
#define WS_EX_NOACTIVATE 0x08000000L
#endif

WinPanel::WinPanel( WinScreen& aScreen, flags type, unsigned long parent ) : winscreen( aScreen )
{
	//this->geometry = new PanelGeometry();

	this->type = type;
	this->winId = null;
	this->mapState = SW_SHOW;
	this->buffer = null;

	DWORD dwExStyle = 0;
	DWORD dwStyle = 0;

	flags panel_type = Panel::PANEL_MASK & type;

	LPCTSTR cn = "OpenOCL_Managed";
	LPCTSTR wn = "Window";

	if ( Panel::POPUP == type )
	{
		cn = "OpenOCL_Popup";
	}

	switch ( panel_type )
	{
	case Panel::MANAGED:
		{
			flags managed_type = Panel::MANAGED_MASK & type;
			switch ( managed_type )
			{
			case APPLICATION:
				dwStyle   = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
				dwExStyle |= WS_EX_COMPOSITED;
				break;
			case DOCUMENT:
				dwStyle = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
				dwExStyle = WS_EX_COMPOSITED;
				//dwExStyle |= WS_EX_MDICHILD;
				break;
			case DIALOG:
				dwStyle = WS_CAPTION | WS_SIZEBOX | WS_OVERLAPPEDWINDOW;
				dwExStyle = WS_EX_COMPOSITED;
				dwExStyle |= WS_EX_DLGMODALFRAME;
				break;
			case FIXED_DIALOG:
				dwStyle = WS_CAPTION; //| WS_DLGFRAME; // | WS_SIZEBOX; //;
				dwExStyle = WS_EX_COMPOSITED;
				dwExStyle |= WS_EX_DLGMODALFRAME;
				break;
			case FLOATING:
				dwStyle = WS_OVERLAPPEDWINDOW | WS_SIZEBOX;
				dwExStyle = WS_EX_COMPOSITED;
				dwExStyle |= WS_EX_TOOLWINDOW;
			}
		}
		break;
	case Panel::UNMANAGED:
		{
			flags unmanaged_type = Panel::UNMANAGED_MASK & type;
			switch ( unmanaged_type )
			{
			case POPUP:
				dwStyle    = WS_POPUP;
				dwExStyle  = WS_EX_COMPOSITED;
				dwExStyle |= WS_EX_TOOLWINDOW;
				break;
			case CHILD:
				dwStyle = WS_CHILD;
				dwExStyle = WS_EX_COMPOSITED;
			}
		}
	}

	int x = CW_USEDEFAULT;
	int y = 100;
	int w = CW_USEDEFAULT;
	int h = 200;

	HWND hWndParent = (HWND) parent;
	HMENU hMenu = 0;
	HINSTANCE hInstance = (HINSTANCE) aScreen.getWinVisual().hInstance;
	LPVOID lpParam = (LPVOID) NULL;

	this->winId = CreateWindowEx( dwExStyle, cn, wn, dwStyle, x, y, w, h, hWndParent, hMenu, hInstance, lpParam );
	if ( this->winId )
	{
		this->winscreen.getWinVisual().windowsUsed++;
	} else {
		FormattedString message( "WinPanel::WinPanel: Out of memory (resouces used: %i)", this->winscreen.getWinVisual().windowsUsed );
		throw new RuntimeException( message );
	}

	EventSource& es = this->winscreen.getWinVisual().getEventSource();
	es.registerDispatcher( this->winId, *this );

	this->gc = new WinGraphicsContext( *this );
	this->status = 0; //( Panel::RELAYOUT | Panel::REDRAW | Panel::REPAINT );
	this->drawnBuffer = false;

	WINDOWINFO winfo;
	winfo.cbSize = sizeof( WINDOWINFO );
	if ( GetWindowInfo( (HWND) this->winId, &winfo ) )
	{
		if ( (winfo.dwExStyle & WS_EX_COMPOSITED) == WS_EX_COMPOSITED )
		{
			this->doubleBuffer = false;
		} else {
			this->doubleBuffer = true;
		}
	} else {
		fprintf( stderr, "WinPanel::WinPanel(): Could not get window info\n" );
		throw new RuntimeException( "WinPanel::WinPanel(): Could not get window info" );
		this->doubleBuffer = true;
	}
	this->isMapped = false;
}

WinPanel::~WinPanel()
{
	//	Need to destroy the window before doing anything
	//	else as it will cause a ResizeEvent to be sent
	//	to the window.
	DestroyWindow( (HWND) this->winId );
	this->winscreen.getWinVisual().windowsUsed--;

	EventSource& es = this->winscreen.getWinVisual().getEventSource();
	es.deregisterDispatcher( this->winId );

	//delete this->geometry;
	delete this->gc;
	if ( this->buffer )
	{
		delete this->buffer;
		this->winscreen.getWinVisual().buffersUsed--;
	}
}

//-------------------------------------------------------------------
//	public virtual methods (EventDispatcher)
//-------------------------------------------------------------------


/*
void WinPanel::dispatchEvent( openocl::base::event::Event* anEvent )
{
	int event_type = anEvent->getType();

	//fprintf( stderr, "WinPanel::dispatchEvent(): target: %x\n", anEvent->getTargetId() );

	switch ( event_type )
	{
	case Event::REPOSITION_EVENT:
		{
			RepositionEvent* re = dynamic_cast<RepositionEvent*>( anEvent );
			this->repositioned( *re );
		}
		break;
	case Event::RESIZE_EVENT:
		{
			ResizeEvent* re = dynamic_cast<ResizeEvent*>( anEvent );
			this->resized( *re );
			this->doLayout( *this->gc );
			this->redraw( *this->gc );
		}
		break;
	case Event::EXPOSE_EVENT:
		this->paint( *this->gc );
		break;
	case Event::CLOSE_EVENT:
		this->forwardEvent( *anEvent );

		//	Is this needed.
		//  this->fireEvent( ActionEvent( *this, 0, "openocl::ui::Panel", "Closed" ) );
		break;
	default:
		switch ( Event::POINTER_EVENT & event_type )
		{
		case Event::POINTER_EVENT:
			PointerEvent* pe = dynamic_cast<PointerEvent*>( anEvent );
			if ( pe )
			{
				int panel_x = this->getAbsoluteX();
				int panel_y = this->getAbsoluteY();
				int panel_relative_x = pe->getX() + this->clientXOffset;
				int panel_relative_y = pe->getY() + this->clientYOffset;
				int event_absolute_x = panel_x + panel_relative_x;
				int event_absolute_y = panel_y + panel_relative_y;

				pe->setAbsoluteCoordinates( event_absolute_x, event_absolute_y );
			}
		}
		this->Container::forwardEvent( *anEvent );
		this->update();
	}
	delete anEvent;
}
*/

//-------------------------------------------------------------------
//	public virtual methods (Component)
//-------------------------------------------------------------------

void
WinPanel::add( Component* aComponent )
{
	//aComponent->setContainingPanel( *this );
	this->Container::add( aComponent );
}

void
WinPanel::add( Component& aComponent )
{
	//aComponent.setContainingPanel( *this );
	this->Container::add( aComponent );
}

void WinPanel::processEvent( const openocl::base::event::Event& anEvent )
{
	/*
	switch ( anEvent.getType() )
	{
	case Event::MOTION_EVENT:
		//fprintf( stderr, "WinPanel::processEvent: MOTION_EVENT\n" );
		break;
	case Event::MOUSE_EVENT:
		//fprintf( stderr, "WinPanel::processEvent: MOUSE_EVENT\n" );
		break;
	}
	*/
}

void WinPanel::setBounds( int x, int y, unsigned int width, unsigned int height )
{
	unsigned int border_width = 0;
	//fprintf( stderr, "%s( %i ).WinPanel::setBounds( %i, %i, %u, %u )\n", this->getName().getChars(), this->getId(), x, y, width, height );
	this->Component::setBounds( x, y, width, height );
	this->requestGeometry( x, y, width, height, 0 );
}

void
WinPanel::setName( const openocl::base::String& aName )
{
	this->Container::setName( aName );
	SetWindowText( (HWND) this->winId, aName.getChars() );
}

	//	public virtual methods (Panel)
Panel*
WinPanel::createChildPanel( flags type ) const
{
	Panel* child_panel = new WinPanel( this->winscreen, type, (unsigned long) this->winId );
	child_panel->show( Panel::SHOW );
	return child_panel;
}

Panel*
WinPanel::createSubPanel() const
{
	Panel* child_panel = new WinPanel( this->winscreen, Panel::CHILD, (unsigned long) this->winId );
	child_panel->show( Panel::SHOW );
	return child_panel;
}

void WinPanel::clear() {}

void
WinPanel::requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border )
{
	HWND win_id = (HWND) this->winId;
	RECT window;
	RECT client;

	GetWindowRect( win_id, &window );
	GetClientRect( win_id, &client );

	int window_width = window.right - window.left;
	int client_width = client.right - client.left;
	int window_height = window.bottom - window.top;
	int client_height = client.bottom - client.top;
	int width_diff = window_width - client_width;
	int height_diff = window_height - client_height;

	unsigned int w = width + width_diff;
	unsigned int h = height + height_diff;

	MoveWindow( (HWND) this->winId, x, y, w, h, true );
}

void WinPanel::requestPosition( int x, int y )
{
	PanelGeometry& geometry = this->getOuterGeometry();
	unsigned int width  = geometry.width;
	unsigned int height = geometry.height;
	MoveWindow( (HWND) this->winId, x, y, width, height, true );
}

void WinPanel::requestSize( unsigned int width, unsigned int height )
{
	PanelGeometry& geometry = this->getOuterGeometry();
	int x = geometry.x;
	int y = geometry.y;
	MoveWindow( (HWND) this->winId, x, y, width, height, true );
}

void
WinPanel::requestSizeLimits( openocl::ui::Dimensions minimum, openocl::ui::Dimensions maximum )
{}

void WinPanel::resetCursor() {}

void
WinPanel::setAlwaysOnTop( bool alwaysOnTop )
{
	const PanelGeometry& client = this->getInnerGeometry();

	if ( alwaysOnTop )
	{
		SetWindowPos( (HWND) this->winId, HWND_TOPMOST, client.x, client.y, client.width, client.height, 0 ); 
	}
	else
	{
		SetWindowPos( (HWND) this->winId, HWND_NOTOPMOST, client.x, client.y, client.width, client.height, 0 ); 
	}
}

void WinPanel::setCursor( unsigned int shape )
{
	//HANDLE cursor = LoadImage( 0, MAKEINTRESOURCE( OCR_NORMAL ), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );

	HCURSOR cursor = LoadCursor( NULL, IDC_IBEAM );
	SetCursor( cursor );
}


	void WinPanel::setCursor( const Image& anImage, const Image& aMask, int hotspotX, int hotspotY ) {}

void
WinPanel::map()
{
	this->status = ( Panel::RELAYOUT | Panel::REDRAW | Panel::REPAINT );
	ShowWindow( (HWND) this->winId, this->mapState );
	UpdateWindow( (HWND) this->winId );
}

void
WinPanel::unmap()
{
	ShowWindow( (HWND) this->winId, SW_HIDE );
}

void
WinPanel::raise()
{
	BringWindowToTop( (HWND) this->winId );
}

void
WinPanel::show( int state )
{
	int nCmdShow = 0;

	switch ( state )
	{
	case SHOW:
		nCmdShow = SW_SHOWNA;
		nCmdShow = SW_SHOWNOACTIVATE;
		this->isMapped = true;
		break;
	case RESTORED:
		nCmdShow = SW_RESTORE;
		this->isMapped = true;
		break;
	case MINIMIZED:
		nCmdShow = SW_MINIMIZE;
		this->isMapped = false;
		break;
	case MAXIMIZED:
		nCmdShow = SW_MAXIMIZE;
		this->isMapped = true;
		break;
	case HIDDEN:
		nCmdShow = SW_HIDE;
		this->isMapped = false;
		break;
	case ACTIVE:
		EnableWindow( (HWND) this->winId, true );
		nCmdShow = SW_SHOW;
		break;
	case INACTIVE:
		EnableWindow( (HWND) this->winId, false );
		nCmdShow = SW_SHOW;
		break;
	}
	ShowWindow( (HWND) this->winId, nCmdShow );
}

void
WinPanel::sendRefreshMessage()
{
	HWND win_id = (HWND) this->winId;
	WinEventManager::SendCustomEvent( (void*) win_id, Event::REFRESH_EVENT );
}	

void
WinPanel::update()
{
	HWND win_id = (HWND) this->winId;

	switch ( this->status )
	{
	case Panel::REFRESH:
		fprintf( stderr, "WinPanel::update: REFRESH shouldn't be called\n" );
		WinEventManager::SendCustomEvent( (void*) win_id, Event::REFRESH_EVENT );
		this->status = Panel::RELAYOUT;
		break;	
	case Panel::RELAYOUT:
		WinEventManager::SendCustomEvent( (void*) win_id, Event::RELAYOUT_EVENT );
		this->status = Panel::REPAINT;
		break;
	case Panel::REDRAW:
		WinEventManager::SendCustomEvent( (void*) win_id, Event::REDRAW_EVENT );
		this->status = Panel::REPAINT;
		break;
	case Panel::REPAINT:
		{
			this->status = 0;

			//	Put in to invalidate entire screen every update.
			//	In the future may change to only update certain
			//	regions.

			Region bounds = this->getBounds();
			RECT rect;
			rect.left = bounds.x1;
			rect.top  = bounds.y1;
			rect.right = bounds.x2;
			rect.bottom = bounds.y2;
			InvalidateRect( win_id, &rect, false );

			//	UpdateWidow causes a WM_PAINT message to be sent to
			//	this Panel, which is needed in Windows to repaint the window.
			UpdateWindow( win_id );
		}
	}
}

/*
void WinPanel::refresh()
{
	HWND win_id = (HWND) this->winId;
	
	//{
	//	PanelGeometry& geometry = this->getOuterGeometry();
	//	short low  = geometry.width;
	//	short high = geometry.height;
	//	LPARAM lParam = MAKELPARAM(low,high);
	//	WPARAM wParam = SIZE_RESTORED;
	//
	//	this->status = Panel::REPAINT;
	//	SendMessage( win_id, WM_SIZE, wParam, lParam );
	//}

	WinEventManager::SendCustomEvent( (void*) win_id, Event::REFRESH_EVENT );
	this->repaint();
	//this->update();

//	{
//		//	Put in to invalidate entire screen every update.
//		//	Is it needed? For now just used for debugging however.
//		RECT rect;
//		rect.left = this->getX();
//		rect.top  = this->getY();
//		rect.right = rect.left + this->getWidth();
//		rect.bottom = rect.top + this->getHeight();
//
//		UpdateWindow( win_id );
//		InvalidateRect( win_id, &rect, true );
//		//SendMessage( win_id, WM_PAINT, 0, 0 );
//	}
}
*/

void
WinPanel::flush() const
{}

Coordinates
WinPanel::queryOuterCoordinates() const
{
	Coordinates c;

	RECT rect;
	GetWindowRect( (HWND) this->winId, &rect );

	c.x = rect.left;
	c.y = rect.top;

	return c;
}

openocl::ui::PanelGeometry
WinPanel::queryOuterGeometry() const
{
	openocl::ui::PanelGeometry g;

	//RECT rect;
	//GetWindowRect( (HWND) this->winId, &rect );

	WINDOWINFO win_info;
	win_info.cbSize = sizeof( WINDOWINFO );
	GetWindowInfo( (HWND) this->winId, &win_info );
	RECT rect = win_info.rcClient;

	g.x = rect.left;
	g.y = rect.top;
	g.width = rect.right - rect.left;
	g.height = rect.bottom - rect.top;
	g.border = win_info.cxWindowBorders;

	return g;
}

openocl::ui::PanelGeometry
WinPanel::queryInnerGeometry() const
{
	openocl::ui::PanelGeometry g;

	WINDOWINFO win_info;
	win_info.cbSize = sizeof( WINDOWINFO );
	GetWindowInfo( (HWND) this->winId, &win_info );

	RECT outer = win_info.rcWindow;
	RECT inner = win_info.rcClient;
	g.x = inner.left - outer.left;
	g.y = inner.top - outer.top;
	g.width = inner.right - inner.left;
	g.height = inner.bottom - inner.top;

	return g;
}


/*
void WinPanel::repaint()
{
	bool draw_again = false;

	//IO::err().printf( "XPanel::repaint()\n" );

	RECT rect;
	rect.left = this->getX();
	rect.top  = this->getY();
	rect.right = rect.left + this->getWidth();
	rect.bottom = rect.top + this->getHeight();

	do {
		//	Invalidates entire screen whenever a repaint occurs.
		InvalidateRect( (HWND) this->winId, &rect, true );
		draw_again = this->redraw( *this->gc );
		this->paint( *this->gc );
	} while ( draw_again );
}
*/

//------------------------------------------------------------------
//	public virtual constant methods (Component)
//------------------------------------------------------------------
int
WinPanel::getAbsoluteX() const
{
	return this->getOuterCoordinates().x;

	/*
	const Panel* panel;
	int absolute_x = 0;
	try
	{
		absolute_x += this->getContainingPanel().getAbsoluteX();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_x += this->geometry->x;
	
	return absolute_x;
	*/
}

int
WinPanel::getAbsoluteY() const
{
	return this->getOuterCoordinates().y;

	/*
	const Panel* panel;
	int absolute_y = 0;
	try
	{
		absolute_y += this->getContainingPanel().getAbsoluteY();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_y += this->geometry->y + this->getY();
	
	return absolute_y;
	*/
}

bool
WinPanel::draw( GraphicsContext& gc ) const
{
	//	If you experience problems with parts of your panel not clearing
	//	you will probably look here as you expect that this is where it
	//	should be cleared.
	//
	//	Actually draw() has no affect on Panels. Panels are redrawn by
	//	having REDRAW and PAINT events dispatched to them. Which causes
	//	redraw( GraphicsContext& ) and repaint( GraphicsContext ) to be
	//	called.

	return false;
}

/*
Dimensions
WinPanel::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	const WinGraphicsContext& win_gc = dynamic_cast<const WinGraphicsContext&>( gc );
	win_gc.beginPaint( NULL );
	dim = this->Panel::getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
	win_gc.endPaint( NULL );
	return dim;
}
*/

Dimensions
WinPanel::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d;
	WinGraphicsContext& win_ddc = const_cast<WinGraphicsContext&>( dynamic_cast<const WinGraphicsContext&>( gc ) );
	win_ddc.beginPaint( NULL );
	win_ddc.resetClip();
	{
		d = this->layoutManager->preferredLayoutDimensions( gc, *this, width, height );
		d.width += (2 * this->getOuterGeometry().border);
 		d.height += (2 * this->getOuterGeometry().border);
	
		if ( d.width < 1 )  d.width = 1;
		if ( d.height < 1 ) d.height = 1;
	}
	win_ddc.endPaint( NULL );

	return d;
}

//---------------------------------------------------------------
//	public  constant methods (Drawable)
//---------------------------------------------------------------

unsigned long
WinPanel::getId() const
{
	return reinterpret_cast<unsigned long>( this->winId );
}

unsigned long
WinPanel::getScreenId() const
{
	return 0;
}

	//	public virtual constant methods (EventDispatcher)
long unsigned int
WinPanel::getEventDispatcherId() const
{
	return (unsigned long) this->winId;
}

	//	public virtual constant methods (Panel)
void
WinPanel::setName( const openocl::base::String& aName ) const
{
	const_cast<WinPanel*>( this )->setName( aName );
}

const Visual&
WinPanel::getVisual() const
{
	return this->winscreen.getWinVisual();
}

const Screen&
WinPanel::getScreen() const
{
	return this->winscreen;
}

Visual&
WinPanel::getVisual()
{
	return this->winscreen.getWinVisual();
}

Screen&
WinPanel::getScreen()
{
	return this->winscreen;
}

GraphicsContext&
WinPanel::getGraphicsContext() const
{
	return *this->gc;
}

unsigned int
WinPanel::getDrawableWidth() const
{
	return this->getWidth();
}

unsigned int
WinPanel::getDrawableHeight() const
{
	return this->getHeight();
}

unsigned int
WinPanel::getDrawableDepth() const
{
	return 0;
}

//-------------------------------------------------------------------
//	public abstract constant methods (Panel)
//-------------------------------------------------------------------

bool
WinPanel::isVisible() const
{
	return this->isMapped;
}


//-------------------------------------------------------------------
//	private methods (WinPanel)
//-------------------------------------------------------------------

/*
 *	A reposition event has been dispatched to this panel. Therefore
 *  change the geometry (details of outer window position/size) and
 *  the bounds (inner client window position/size).
 */

/**
void
WinPanel::repositioned( const Event& anEvent )
{
	const RepositionEvent& rep = dynamic_cast<const RepositionEvent&>( anEvent );

	//	The geometry represents the outer edge of the window.
	PanelGeometry& outer = this->getOuterGeometry();
	PanelGeometry& inner = this->getInnerGeometry();
	outer.x = rep.getX();
	outer.y = rep.getY();

	WINDOWINFO win_info;
	win_info.cbSize = sizeof( WINDOWINFO );
	GetWindowInfo( (HWND) this->winId, &win_info );
	RECT rect = win_info.rcClient;
	inner.x = rect.left - outer.x;
	inner.y = rect.top  - outer.y;

	fprintf( stderr, "WinPanel::repositioned( %x, %i, %i )\n", anEvent.getTargetId(), outer.x, outer.y );

	//	The client window is always relative to itself therefore we
	//	do not need to change the bounds.
}

void
WinPanel::resized( const Event& anEvent )
{
	const ResizeEvent& resize = dynamic_cast<const ResizeEvent&>( anEvent );

	//	The geometry represents the outer edge of the window.
	PanelGeometry& outer = this->getOuterGeometry();
	PanelGeometry& inner = this->getInnerGeometry();
	outer.width  = resize.getWidth();
	outer.height = resize.getHeight();

	//	The bounds represent the inner client area of the window
	WINDOWINFO win_info;
	win_info.cbSize = sizeof( WINDOWINFO );
	GetWindowInfo( (HWND) this->winId, &win_info );

	RECT rect = win_info.rcClient;
	unsigned int current_width = this->getWidth();
	unsigned int current_height = this->getHeight();

	unsigned int bounds_width  = rect.right - rect.left;
	unsigned int bounds_height = rect.bottom - rect.top;

	inner.width = bounds_width;
	inner.height = bounds_height;

	if ( (current_width != bounds_width) || (current_height != bounds_height) )
	{
		this->Container::setBounds( 0, 0, bounds_width, bounds_height );
	}

	//	Initialize client window offsets so that absolute position can
	//	be properly calculated later.
	{
		int win_width = win_info.rcWindow.right - win_info.rcWindow.left;
		int win_height = win_info.rcWindow.bottom - win_info.rcWindow.top;

		this->clientXOffset = win_width - win_info.cxWindowBorders - bounds_width;
		this->clientYOffset = win_height - win_info.cyWindowBorders - bounds_height;
	}
}
**/

void
WinPanel::doLayout( const GraphicsContext& gc )
{
	WinGraphicsContext& win_ddc = const_cast<WinGraphicsContext&>( dynamic_cast<const WinGraphicsContext&>( gc ) );
	win_ddc.beginPaint( NULL );
	win_ddc.resetClip();
	Container::doLayout( gc );
	win_ddc.endPaint( NULL );
}


void
WinPanel::redraw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	//if ( !this->buffer )
	//{
	//	const_cast<WinPanel*>( this )->buffer = dynamic_cast<WinPixmap*>( gc.createOffscreenImage( 2000, 2000, NULL ) );
	//	if ( this->buffer )
	//	{
	//		this->temporyBuffer = true;
	//	} else {
	//		abort();
	//	}
	//}

	if ( this->doubleBuffer )
	{
		if ( this->buffer )
		{
			WinGraphicsContext& bgc = dynamic_cast<WinGraphicsContext&>( this->buffer->getGraphicsContext() );
			//gc.clear();
			draw_again |= this->Container::draw( bgc );
		}
	}
	else
	{
		//	This is executed during paint anyway.
		//
		//WinGraphicsContext& winDDC = dynamic_cast<WinGraphicsContext&>( gc );
		//PAINTSTRUCT lpPaint;
		//winDDC.beginPaint( &lpPaint );
		//winDDC.resetClip();
		//gc.clear();
		//draw_again |= this->Container::draw( gc );
		//winDDC.endPaint( &lpPaint );
	}
}

void
WinPanel::paint( GraphicsContext& gc ) const
{
	WinGraphicsContext& winDDC = dynamic_cast<WinGraphicsContext&>( gc );
	PAINTSTRUCT lpPaint;
	winDDC.beginPaint( &lpPaint );
	winDDC.resetClip();

	//const_cast<WinPanel*>( this )->forwardRedrawAllChangeEvent();

	if ( this->doubleBuffer )
	{
		//	RECT rect;
		//	rect.left = this->getX();
		//	rect.top  = this->getY();
		//	rect.right = rect.left + this->getWidth();
		//	rect.bottom = rect.top + this->getHeight();
		//	InvalidateRect( (HWND) this->winId, &rect, true );

		if ( !this->buffer )
		{
			WinPanel* rw = const_cast<WinPanel*>( this );
			rw->buffer = dynamic_cast<WinPixmap*>( gc.createOffscreenImage( 1280, 1024, NULL ) );
			if ( rw->buffer )
			{
				this->winscreen.getWinVisual().buffersUsed++;
			} else {
				FormattedString message( "WinPanel::paint: Out of memory (resouces used: %i)", this->winscreen.getWinVisual().buffersUsed );
				throw new RuntimeException( message );
			}
			GraphicsContext& bgc = rw->getGraphicsContext();
			this->redraw( bgc );
		}

		if ( this->buffer )
		{
			unsigned int target_width  = this->getDrawableWidth();
			unsigned int target_height = this->getDrawableHeight();
			gc.copyArea( *this->buffer, 0, 0, target_width, target_height, 0, 0 );
		}
	} else {
		//gc.setForeground( Color::WHITE );
		//gc.setFill( Color::WHITE );
		//gc.drawFilledRectangleRegion( this->getBounds() );
		//draw_again |= this->Component::draw( gc );

		//gc.clear();
		this->Container::draw( gc );

		//winDDC.drawString( "Hello world!", 0, 0 );
	}

	gc.flush();
	winDDC.endPaint( &lpPaint );
}

//  virtual const AbstractDisplay& getAbstractDisplay() const = 0 {}
//  virtual unsigned int getScreen() const = 0 {}

void
WinPanel::calculateAbsoluteCoordinatesOf( PointerEvent& pe ) const
{
	int panel_x = this->getAbsoluteX();
	int panel_y = this->getAbsoluteY();
	int panel_relative_x = pe.getX() + this->clientXOffset;
	int panel_relative_y = pe.getY() + this->clientYOffset;
	int event_absolute_x = panel_x + panel_relative_x;
	int event_absolute_y = panel_y + panel_relative_y;

	pe.setAbsoluteCoordinates( event_absolute_x, event_absolute_y );
}


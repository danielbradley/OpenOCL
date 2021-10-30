/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <X11/Xlib.h>
#include <X11/cursorfont.h>
#include "openocl/ui/Xincludes.h"
#undef Visual

#include "openocl/ui/X11/XPanel.h"
#include "openocl/ui/PanelGeometry.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/Menu.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Visual.h"
#include "openocl/ui/X11/XEventManager.h"
#include "openocl/ui/X11/XGraphicsContext.h"
#include "openocl/ui/X11/XPixmap.h"
#include "openocl/ui/X11/XScreen.h"
#include "openocl/ui/X11/XVisual.h"

#include <openocl/base/Debug.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/ConfigureEvent.h>
#include <openocl/base/event/ExposeEvent.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/RepositionEvent.h>
#include <openocl/base/event/ResizeEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

//  Uncomment to get debug statements or define using cc <-D>
//  #define OPENOCL_UI_X11_XPANEL_DEBUG

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::util;
using namespace openocl::ui;
using namespace openocl::ui::X11;


XPanel::XPanel( XScreen& aScreen, int type, unsigned long parentId )
: Panel(), xscreen( aScreen )
{
	XSetWindowAttributes* window_attributes = (XSetWindowAttributes*) Runtime::calloc( 1, sizeof( XSetWindowAttributes ) );
	unsigned long valuemask = 0;

	Display* dpy = (Display*) this->xscreen.getXDisplay();
	this->display = (void*) dpy;
	Window wnd = 0;
	this->screen = aScreen.getScreenId();

//	this->geometry.x = 0;
//	this->geometry.y = 0;
//	this->geometry.width = 10;
//	this->geometry.height = 10;

	if ( Panel::FIXED_DIALOG == type )
	{
		this->fixedSize = true;
	} else {
		this->fixedSize = false;
	}

	switch ( type )
	{
	case APPLICATION:
	case DOCUMENT:
	case DIALOG:
	case FIXED_DIALOG:
	case Panel::FLOATING:
		window_attributes->override_redirect = False;
		break;
	case POPUP:
		window_attributes->override_redirect = True;
		valuemask |= CWOverrideRedirect;
		//window_attributes.background_pixel = None;
		//window_attributes.backing_pixel = None;
		//XChangeWindowAttributes( dpy, this->winId, CWOverrideRedirect, &window_attributes );
		//XChangeWindowAttributes( dpy, this->winId, CWOverrideRedirect | CWBackPixel, &window_attributes );
		break;
	}
	
	window_attributes->bit_gravity = NorthWestGravity;
	valuemask |= CWBitGravity;

	int x = 0;
	int y = 0;
	unsigned int w = 1;
	unsigned int h = 1;
	unsigned int bw = 0;
	wnd = XCreateWindow( dpy, parentId, x, y, w, h, bw,
				         CopyFromParent,			// depth
				         InputOutput,			// class
				         CopyFromParent,			// visual
				         valuemask,
						window_attributes );
	Runtime::free( window_attributes );

	XSelectInput( dpy, wnd, ExposureMask
			| KeyPressMask
			| KeyReleaseMask
			| ButtonPressMask
			| ButtonReleaseMask
			| ButtonMotionMask
			| PointerMotionMask
			| StructureNotifyMask
			| FocusChangeMask );

	//  fprintf( stderr, "XPanel::XPanel(): created window (%x)\n", wnd );

	this->winId = wnd;
	this->isMapped = false;
	this->setHints( type );

	switch ( type )
	{
	case APPLICATION:
		break;
	case DIALOG:
	case FIXED_DIALOG:
		XSetTransientForHint( dpy, this->winId, parentId );
		break;
	case FLOATING:
		break;
	}

	this->depth = DefaultDepth( dpy, screen );

	this->gc = new XGraphicsContext( *this );
	this->buffer = null;

	{
		EventSource& es = this->xscreen.getXVisual().getEventSource();
		EventDispatcher& e = *this;
		es.registerDispatcher( (void*) this->winId, e );
	}
}

XPanel::~XPanel()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	{
		EventSource& es = this->xscreen.getXVisual().getEventSource();
		es.deregisterDispatcher( (void*) this->winId );
	}

	delete this->gc;
	delete this->buffer;

	//  fprintf( stderr, "XPanel::~XPanel(): destroying window (%x)\n", this->winId );
	XDestroyWindow( dpy, this->winId );
}

//-------------------------------------------------------------------------------------------
//	public virtual methods (EventDispatcher)
//-------------------------------------------------------------------------------------------

/*
void
XPanel::dispatchEvent( Event* anEvent )
{
	flags type = anEvent->getType();

	//fprintf( stderr, "XPanel::dispatchEvent( Event* )\n" );

	switch ( Event::WINDOW_EVENT & type )
	{
	case Event::WINDOW_EVENT:
		{
			//fprintf( stderr, "Window event: %x\n", type );
			switch ( type )
			{
			case Event::REPOSITION_EVENT:
				{
					const RepositionEvent* re = dynamic_cast<const RepositionEvent*>( anEvent );
					this->repositioned( *re );
					//this->setGeometryAndBounds( *anEvent );
					this->paint( *this->gc );
				}
				break;
			case Event::RESIZE_EVENT:
				{
					const ResizeEvent* re = dynamic_cast<const ResizeEvent*>( anEvent );
					///if ( this->resized( *re ) )
					this->resized( *re );
					{
						//this->setGeometryAndBounds( *re );
					
						this->doLayout( *this->gc );
						this->redraw( *this->gc );
						this->paint( *this->gc );
					}
				}
				break;
			case Event::EXPOSE_EVENT:
				{
					//ExposeEvent* expose_event = dynamic_cast<ExposeEvent*>( anEvent );
					//if ( expose_event && (expose_event->getCount() == 0) )
					//{
						//	this->doLayout( *this->gc );
						//	this->redraw( *this->gc );
						this->paint( *this->gc );
					//}
				}
				break;
			case Event::CLOSE_EVENT:
				this->forwardEvent( *anEvent );
				break;
			case Event::FOCUS_EVENT:
				this->Container::forwardEvent( *anEvent );
				
			}
		}
		break;
	default:
		switch ( Event::POINTER_EVENT & type )
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
		//IO::err().printf( "XPanel::dispatchEvent: type: %u\n", anEvent->getType() ); 

		if ( Event::ACTION_EVENT == type )
		{
			//  fprintf( stderr, "XPanel::dispatchEvent( ACTION_EVENT )\n" );
			this->fireEvent( *anEvent );
		} else {
			//  fprintf( stderr, "XPanel::dispatchEvent(): forwarding pointer event\n" );
			this->Container::forwardEvent( *anEvent );
		}
	}
	this->update();
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XFlush( dpy );
	delete anEvent;
}
*/

//-------------------------------------------------------------------------------------------
//	public virtual methods (Component)
//-------------------------------------------------------------------------------------------

//
//	Not really sure what I intended this to be for.
//	I think it is so subclasses can override it so
//	that they can easily handle events being received
//	by the XPanel
//
void
XPanel::processEvent( const Event& anEvent )
{
	//IO::err().printf( "XPanel::processEvent()\n" );
	if ( Event::FOCUS_EVENT == anEvent.getType() ) {
		//IO::err().printf( "XPanel::processEvent( FocusEvent )\n" );
	}
	//this->redraw( this->getGraphicsContext() );
	//this->update();
}

/**
 *  The setBounds method is called by the Layout Manager of the
 *  containing Container. For normal components it sets the
 *  x, y, width, and height members of a class that are used
 *  during drawing to position the component. However, a panel
 *  represents a new window that resets the origin to its
 *  top left corner. Therefore, the bounds need to be set as
 *  0, 0, width, height.
 *
 *  First this method repositions the window to where its
 *  container whats it.
 *
 *  Then it sets the bounds.
 *
 *  Then is stores the absoluteX, and absouteY coordinates.
 */  
void
XPanel::setBounds( int x,
                   int y,
                   unsigned int width,
                   unsigned int height )
{
	//	When a panel changes its bounds it only changes the contents
	//	if the width or height changes.

	Region bounds = this->getBounds();

#ifdef OPENOCL_UI_X11_OPTIMIZE_SETBOUNDS

	//	There seems to be a problem with this, when used the PanelScroller
	//	does not correctly draw a panel scrolled to 0 offset. 

	if ( (bounds.width != ((int)width)) || (bounds.height != ((int)height)) )
	{
		this->Container::setBounds( x, y, width, height );
		this->requestXWindowPosition( x, y );
		this->requestXWindowSize( width, height );
		this->requestXWindowBorder( 0 );
	} else if ( (bounds.x1 != x) || (bounds.y1 != y) ) {
		this->requestXWindowPosition( x, y );
	}
#else
	this->Container::setBounds( x, y, width, height );
	this->requestXWindowPosition( x, y );
	this->requestXWindowSize( width, height );
	this->requestXWindowBorder( 0 );
#endif


/*
  Debug::entering( "XPanel", "setBounds", "..." );
  

  this->Component::setBounds( 0, 0, aWidth, aHeight );

  StringBuffer sb;
  sb.append( String( "%u", anX ) );
  sb.append( ", " );
  sb.append( String( "%u", aY ) );
  sb.append( ", " );
  sb.append( String( "%u", aWidth ) );
  sb.append( ", " );  
  sb.append( String( "%u", aHeight ) );
  Debug::println( sb.toString() );

  unsigned int borderWidth = this->geometry.borderWidth;

  int internal_width  = aWidth  - (borderWidth*2);
  int internal_height = aHeight - (borderWidth*2);

	if ( 0 > internal_width ) internal_width = 0;  
	if ( 0 > internal_height ) internal_height = 0;  

  this->setGeometry( anX, aY, internal_width, internal_height, borderWidth );

  Debug::leaving();
*/
}

void
XPanel::setName( const openocl::base::String& aName )
{
	this->Component::setName( aName );

	Display* dpy = (Display*) this->xscreen.getXDisplay();
	char* window_name = String::copy( aName.getChars() );
	{
		XTextProperty windowName;
		XStringListToTextProperty( &window_name, 1, &windowName );
		XSetWMName( dpy, this->winId, &windowName );
		XFree( windowName.value );
	}
	delete window_name;
}

//-------------------------------------------------------------------------------------------
//	public virtual methods (Container)
//-------------------------------------------------------------------------------------------

void
XPanel::add( Component* aComponent )
{
	this->Container::add( aComponent );
}

void
XPanel::add( Component& aComponent )
{
	this->Container::add( aComponent );
}

//-------------------------------------------------------------------------------------------
//	public virtual methods (Panel)
//-------------------------------------------------------------------------------------------





void
XPanel::clear()
{
	unsigned int w = this->getDrawableWidth();
	unsigned int h = this->getDrawableHeight();

	this->gc->resetClip();
	this->gc->setClip( 0, 0, w, h );

	this->gc->setForeground( Color::WHITE );
	this->gc->setFill( Color::WHITE );
	this->gc->setBackground( Color::WHITE );
	this->gc->drawFilledRectangle( 0, 0, w, h );
}

void
XPanel::requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth )
{
	//fprintf( stderr, "%s.XPanel::requestGeometry( %i, %i, %u, %u )\n", this->getName().getChars(), x, y, width, height );
	PanelGeometry& outer = this->getOuterGeometry();
	this->requestXWindowPosition( x, y );
	this->requestXWindowSize( width, height );
	this->requestXWindowBorder( borderWidth );

	outer.x = x;
	outer.y = y;
	outer.width = width;
	outer.height = height;
	outer.border = borderWidth;
}

void
XPanel::requestPosition( int x, int y )
{
	//fprintf( stderr, "%s.XPanel::requestPosition( %i, %i )\n", this->getName().getChars(), x, y );
	PanelGeometry& outer = this->getOuterGeometry();
	outer.x = x;
	outer.y = y;
	this->requestXWindowPosition( x, y );
} 

void
XPanel::requestSize( unsigned int width, unsigned int height )
{
	PanelGeometry& outer = this->getOuterGeometry();
	outer.width = width;
	outer.height = height;
	
	this->requestXWindowSize( width, height );
} 

void
XPanel::resetCursor()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XUndefineCursor( dpy, this->winId );
}

void
XPanel::setAlwaysOnTop( bool alwaysOnTop )
{
	if ( alwaysOnTop )
	{
	
	
	}
}

void
XPanel::setCursor( unsigned int shape )
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Cursor cursor = XCreateFontCursor( dpy, shape );
	XDefineCursor( dpy, this->winId, cursor );
}

void
XPanel::setCursor( const openocl::imaging::Image& anImage, const openocl::imaging::Image& aMask, int hotspotX, int hotspotY )
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();

	//Colormap colormap = DefaultColormap( dpy, screen );
	
	//XColor black, white;

	//XPixmap* shape  = dynamic_cast<XPixmap*>( this->gc->createPixmap( anImage ) );
	//XPixmap* mask   = dynamic_cast<XPixmap*>( this->gc->createPixmap( aMask ) );

	//XParseColor( dpy, colormap, "black", &black );
	//XParseColor( dpy, colormap, "white", &white );

	//Cursor cursor = XCreatePixmapCursor( dpy, shape->getId(), mask->getId(), &black, &white, hotspotX, hotspotY );
	//Cursor cursor = XCreatePixmapCursor( dpy, shape->getId(), mask->getId(), 0x0000, 0xFFFFFF, hotspotX, hotspotY );
	
	Cursor cursor = XCreateFontCursor( dpy, XC_coffee_mug );
	XDefineCursor( dpy, this->winId, cursor );
}

void
XPanel::map()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XMapWindow( dpy, this->winId );
	XFlush( dpy );
	this->isMapped = true;
}

void
XPanel::unmap()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XUnmapWindow( dpy, this->winId );
	XFlush( dpy );
	this->isMapped = false;
}

void
XPanel::raise()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XRaiseWindow( dpy, this->winId );
	XFlush( dpy );
}

void
XPanel::show( int state )
{
#ifdef DEBUG_OPENOCL_UI_X11_XPANEL
	fprintf( stderr, "XPanel::show()\n" );
#endif
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	switch ( state )
	{
	case SHOW:
	case RESTORED:
		{
			//	XXX Kludge
			//
			//	In order to have the panel reposition itself properly where it was
			//	when it was hidden, the window needs to be mapped before moving
			//	the panel to the old location.
			//
			//	I'm not sure at the moment how to fix this.
		
			PanelGeometry outer = this->getOuterGeometry();
#ifdef DEBUG_OPENOCL_UI_X11_XPANEL
			fprintf( stderr, "XPanel::show(): RESTORED: %i, %i, %i, %i, %i\n", outer.x, outer.y, outer.width, outer.height, outer.border );
#endif

			XMapWindow( dpy, this->winId );
			//this->requestGeometry( outer.x, outer.y, outer.width, outer.height, outer.border );
			this->isMapped = true;
		}
		break;
	case MINIMIZED:
		XIconifyWindow( dpy, this->winId, this->screen );
		this->isMapped = false;
		break;
	case MAXIMIZED:
		break;
	case HIDDEN:
		XWithdrawWindow( dpy, this->winId, this->screen );
		this->isMapped = false;
		break;
	case ACTIVE:
		XSelectInput( dpy, this->winId, ExposureMask
	                                | KeyPressMask
					| KeyReleaseMask
					| ButtonPressMask
					| ButtonReleaseMask
					| ButtonMotionMask
					| PointerMotionMask
					| StructureNotifyMask
					| FocusChangeMask );
		this->show( Panel::SHOW );
		break;
	case INACTIVE:
		XSelectInput( dpy, this->winId, ExposureMask | StructureNotifyMask );		
		break;
	}
	XFlush( dpy );
}

void
XPanel::sendRefreshMessage()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Window win_id = (Window) this->winId;
	XEventManager::SendCustomEvent( dpy, win_id, Event::REFRESH_EVENT );
}

/**
 *  The update method is called when an XPanel receives a
 *  ConfigureEvent, which occurs when a window is resized.
 *
 *  The update method calls the <method>setBounds</method>
 *  method of the parent Component class.
 */
void
XPanel::update()
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Window win_id = (Window) this->winId;

#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
	fprintf( stderr, "%s( %i ).XPanel::update: status == 0x%x \n", this->getName().getChars(), this->getId(), this->status );
#endif

	int state = this->status;
	this->status = 0;

	switch ( state )
	{
	case Panel::RELAYOUT:
		{
			XEventManager::SendCustomEvent( dpy, win_id, Event::RELAYOUT_EVENT );
		}
		//	Intentional fall-through to REDRAW
	case Panel::REDRAW:
		{
			XEventManager::SendCustomEvent( dpy, win_id, Event::REDRAW_EVENT );
		}
		//	Intentional fall-through to REPAINT
	case Panel::REPAINT:
		{
			XEventManager::SendCustomEvent( dpy, win_id, Event::EXPOSE_EVENT );
#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
			fprintf( stderr, "XPanel::update(): XExposeEvent[%i:%i]\n", event.width, event.height ); 
#endif
		}
	}
}

void
XPanel::refresh()
{
#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
	fprintf( stderr, "XPanel::refresh()\n" ); 
#endif

	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Window win_id = (Window) this->winId;

	XEventManager::SendCustomEvent( dpy, win_id, Event::REFRESH_EVENT );
	this->Panel::repaint();

#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
	fprintf( stderr, "XPanel::refresh(): leaving\n" ); 
#endif
}

//-------------------------------------------------------------------------------------------
//	public virtual constant methods (Component)
//-------------------------------------------------------------------------------------------

int
XPanel::getAbsoluteX() const
{
	int absolute_x = 0;
	try
	{
		absolute_x = this->getContainingPanel().getAbsoluteX();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_x += this->getOuterCoordinates().x;
	
	return absolute_x;
}

int
XPanel::getAbsoluteY() const
{
	int absolute_y = 0;
	try
	{
		absolute_y = this->getContainingPanel().getAbsoluteY();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	absolute_y += this->getOuterCoordinates().y;
	
	return absolute_y;
}

bool
XPanel::draw( GraphicsContext& aGraphics ) const
{
	//	This is empty as a parent panel
	//	does not need to ask a subpanel to
	//	redraw, it will redraw on its own
	//	when it gets an ExposeEvent.

	return false;
}

Dimensions
XPanel::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d = this->layoutManager->preferredLayoutDimensions( gc, *this, width, height );
	d.width += (2 * this->getOuterGeometry().border);
 	d.height += (2 * this->getOuterGeometry().border);
	
	if ( d.width < 1 )  d.width = 1;
	if ( d.height < 1 ) d.height = 1;

	return d;
}

unsigned int
XPanel::getPreferredWidth( const GraphicsContext& gc ) const
{
	Dimensions dim = this->layoutManager->preferredLayoutDimensions( gc, *this, 0xFFFF, 0xFFFF );
	dim.width += (2 * this->getOuterGeometry().border);
	return dim.width;
}

unsigned int
XPanel::getPreferredHeight( const GraphicsContext& gc ) const
{
	Dimensions dim = this->layoutManager->preferredLayoutDimensions( gc, *this, 0xFFFF, 0xFFFF );
 	dim.height += (2 * this->getOuterGeometry().border);
	return dim.height;
}

//-------------------------------------------------------------------------------------------
//	public virtual constant methods (Drawable)
//-------------------------------------------------------------------------------------------

openocl::ui::Visual&
XPanel::getVisual()
{
	return this->xscreen.getXVisual();
}

const openocl::ui::Visual&
XPanel::getVisual() const
{
	return this->xscreen.getXVisual();
}

GraphicsContext&
XPanel::getGraphicsContext() const
{
  return *this->gc;
}

unsigned long int
XPanel::getId() const
{
	return this->winId;
}

unsigned long
XPanel::getScreenId() const
{
	return this->screen;
}

unsigned int
XPanel::getDrawableWidth() const
{
	return this->getInnerGeometry().width;
}

unsigned int
XPanel::getDrawableHeight() const
{
	return this->getInnerGeometry().height;
}

unsigned int
XPanel::getDrawableDepth() const
{
	return this->depth;
}


//-------------------------------------------------------------------------------------------
//	public abstract constant methods (Panel)
//-------------------------------------------------------------------------------------------

bool
XPanel::isVisible() const
{
	return this->isMapped;
}

//-------------------------------------------------------------------------------------------
//	public virtual constant methods (Panel)
//-------------------------------------------------------------------------------------------

Panel*
XPanel::createChildPanel( flags type ) const
{
	XPanel* subpanel = new XPanel( this->xscreen, type, this->winId );
	subpanel->show( Panel::SHOW );
	return subpanel;
}

Panel*
XPanel::createSubPanel() const
{
	XPanel* subpanel = new XPanel( this->xscreen, 0, this->winId );
	subpanel->show( Panel::SHOW );
	return subpanel;
}

void
XPanel::setName( const openocl::base::String& aName ) const
{
	const_cast<XPanel*>( this )->setName( aName );
}

openocl::ui::Screen&
XPanel::getScreen()
{
	return this->xscreen;
}

const openocl::ui::Screen&
XPanel::getScreen() const
{
	return this->xscreen;
}

void
XPanel::flush() const
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XFlush( dpy );
}

Coordinates
XPanel::queryOuterCoordinates() const
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Window root = RootWindow( dpy, this->screen );
	Window child;
	Coordinates coords;

	XTranslateCoordinates(
		dpy,
		this->winId,
		root,
		0,
		0,
		&coords.x,
		&coords.y,
		&child
	);

	return coords;
}


PanelGeometry
XPanel::queryOuterGeometry() const
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	Window root = RootWindow( dpy, this->screen );
	Window child;
	PanelGeometry outer;
	unsigned int depth;
	
	switch ( 1 )
	{
	case 0:
		XGetGeometry(
			dpy,
			this->winId,
			&root,
			&outer.x,
			&outer.y,
			&outer.width,
			&outer.height,
			&outer.border,
			&depth
		);
		break;
	case 1:
		{
			XWindowAttributes attributes;
			XGetWindowAttributes( dpy, this->winId, &attributes );
			outer.x      = attributes.x;
			outer.y      = attributes.y;
			outer.width  = attributes.width;
			outer.height = attributes.height;
			outer.border = attributes.border_width;

			XTranslateCoordinates(
				dpy,
				this->winId,
				root,
				0,
				0,
				&outer.x,
				&outer.y,
				&child
			);
		}
		break;
	}

#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
	fprintf( stderr, "%s->XPanel::queryOuterGeometry() : %i:%i %i:%i (%i) [%i]\n",
			this->getName().getChars(),
			outer.x,
			outer.y,
			outer.width,
			outer.height,
			outer.border, this->winId );
#endif
	
	return outer;
}

PanelGeometry
XPanel::queryInnerGeometry() const
{
	PanelGeometry outer = this->queryOuterGeometry();
	PanelGeometry inner;

	inner.x = outer.border;
	inner.y = outer.border;
	inner.width  = outer.width - (2*outer.border);
	inner.height = outer.height - (2*outer.border);
	inner.border = outer.border;
	
	return inner;
}


/*
void
XPanel::causerepaint()
{
}
*/
//void
//XPanel::redraw()
//{
//	bool draw_again = false;
//
//	//IO::err().printf( "XPanel::repaint()\n" );
//	
//	do {
//		draw_again = this->redraw( *this->gc );
//		this->paint( *this->gc );
//	} while ( draw_again );
//}	

//-------------------------------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------------------------------

//void
//XPanel::setPanelSize( unsigned int width, unsigned int height )
//{
//	this->panelWidth = width;
//	this->panelHeight = height;
//}

/***********
 *  Depeciated
 
void
XPanel::setGeometryAndBounds( const Event& anEvent )
{
	int type = anEvent.getType();

	Geometry& outer = this->getOuterGeometry();
	Geometry& inner = this->getInnerGeometry();
	
	switch ( type )
	{
	case Event::REPOSITION_EVENT:
		{
			const RepositionEvent& re = dynamic_cast<const RepositionEvent&>( anEvent );
			outer.x = re.getX();
			outer.y = re.getY();
			inner.x = 0;
			inner.y = 20;
			fprintf( stderr, "%s %i:%i\n", this->getName().getChars(), outer.x, outer.y );
		}
		break;
	case Event::RESIZE_EVENT:
		{
			const ResizeEvent& re = dynamic_cast<const ResizeEvent&>( anEvent );
			unsigned int w = re.getWidth();
			unsigned int h = re.getHeight();
			unsigned int b = outer.border;
			
			outer.width  = w;
			outer.height = h;
			if ( !outer.border )
			{
				//	This is a WORK AROUND that fixes a problem with
				//	configure events loosing the border width after
				//	the first configure.
				outer.border = (b = re.getBorderWidth());
			}

			int border_space = b * 2;
			unsigned int component_width;
			unsigned int component_height;
			if ( (w - border_space) < 0 )
			{
				component_width  = w - border_space;
				component_height = h - border_space;

				//	Only top level panels should have their bounds set by
				//	configure events. Lower level panels are actually oversized
				//	and have their bounds artificially constrained to show
				//	the correct size. This is a workaround to stop flicker that
				//	occurs when a Panel is resized using XMoveResizeWindow
			} else {
				component_width = w;
				component_height = h;
			}
			inner.width  = component_width;
			inner.height = component_height;
			
			this->Container::setBounds( b, b, component_width, component_height );
		}
		break;
	}
}

*/

/**
 *  Redraw prepares the object for drawing, then
 *  draws the component. <p>
 *
 *  First it calls doLayout to make sure any
 *  subcomponents are laid out properly, then
 *  it calls draw to draw the components.
 */
void
XPanel::redraw( GraphicsContext& gc ) const
{
	this->reallyDraw( gc );
}

void
XPanel::doLayout( const GraphicsContext& gc )
{
	this->Container::doLayout( gc );

	//Debug::entering( "XPanel", "doLayout", "GraphicsContext&" );
	//this->layoutManager->doLayout( *this, gc );
	//Debug::leaving();
}

void
XPanel::reallyDoLayout( const GraphicsContext& gc )
{
	Debug::entering( "XPanel", "reallyDoLayout", "GraphicsContext&" );
	this->layoutManager->doLayout( *this, *this->gc );
	Debug::leaving();
}

/*
 *  This didn't work. :(
void
XPanel::preDraw()
{
	if ( BUFFERED )
	{
		if ( !this->buffer )
		{
			const_cast<XPanel*>( this )->buffer = new XPixmap( *this, 2000, 2000, this->depth );
		}

		if ( this->buffer )
		{
			this->buffer->clear();
			GraphicsContext& buffer_gc = this->buffer->getGraphicsContext();
			buffer_gc.resetClip();
			this->Container::draw( buffer_gc );
		}
	}
}
*/

bool
XPanel::reallyDraw( GraphicsContext& gc ) const
{
	bool redraw_all = this->isRedrawAll();
	bool draw_again = false;

	if ( !this->buffer )
	{
		const_cast<XPanel*>( this )->buffer = new XPixmap( *this, 2000, 2000, this->depth );
	}

	if ( BUFFERED )
	{
		if ( this->buffer )
		{
			//this->buffer->clear();
			GraphicsContext& buffer_gc = this->buffer->getGraphicsContext();
			if ( redraw_all )
			{
				buffer_gc.clear();
			}
			buffer_gc.resetClip();
			draw_again = this->Container::draw( buffer_gc );
		}
	} else {
		if ( &gc )
		{
			if ( redraw_all )
			{
				gc.clear();
			}
			gc.resetClip();
			draw_again = this->Container::draw( gc );
		}
	}

	return draw_again;
}

void
XPanel::paint( GraphicsContext& onscreen ) const
{
	//  IO::err().printf( "XPanel::paint()\n" );
	//	The paint method repaints the entire buffer to the target window.

#ifdef DEBUG_MEMORY
	fprintf( stderr, "XPanel::paint: >>> objCount: %lli\n", Object::getObjectCount() );
	{
#endif

	if ( BUFFERED && this->buffer )
	{
		XGraphicsContext& gc = dynamic_cast<XGraphicsContext&>( onscreen );
		if ( &gc )
		{
			int width  = this->buffer->getDrawableWidth();
			int height = this->buffer->getDrawableHeight();
			gc.resetClip();
			gc.copyArea( *buffer, 0, 0, width, height, 0, 0 );
		}
	}

#ifdef DEBUG_MEMORY
	}
	fprintf( stderr, "XPanel::paint: <<< objCount: %lli\n", Object::getObjectCount() );
#endif
}


long unsigned int
XPanel::getEventDispatcherId() const
{
  return this->getId();
}

/*
void
XPanel::deliverEvent( const Event& anEvent )
{
	this->fireEvent( anEvent );
	//IO::err().printf( "%s.XPanel::deliverEvent()\n", this->getName().getChars() );

	switch ( anEvent.getType() )
	{
	case Event::CHANGE_EVENT:
		this->redraw( *this->gc );
		this->paint( *this->gc );
		//this->update();
		break;
	};
}
*/



//void
//XPanel::setSize( unsigned int width, unsigned int height )
//{
//  Display* dpy = (Display*) 
//this->xvisual.getXDisplay().getDisplay();
//  XMoveResizeWindow( dpy, this->winId,
//                    this->getX(), this->getY(), width, height );
//}



void
XPanel::copyArea( const Drawable& aDrawable,
                  unsigned int sourceX,
                  unsigned int sourceY,
                  unsigned int sourceWidth,
                  unsigned int sourceHeight,
                  unsigned int destinationX,
                  unsigned int destinationY ) const
{
	this->gc->copyArea( aDrawable, sourceX,
                                 sourceY,
                                 sourceWidth,
                                 sourceHeight,
                                 (XPanel&) *this,
                                 destinationX,
                                 destinationY );
}


const XVisual&
XPanel::getXVisual() const
{
	return this->xscreen.getXVisual();
}

const XScreen&
XPanel::getXScreen() const
{
	return this->xscreen;
}



XScreen&
XPanel::getXScreen()
{
	return this->xscreen;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//	protected methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void
XPanel::setHints( int type )
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	char* window_name = String::copy( this->getName().getChars() );

	Atom atom = XInternAtom( dpy, "WM_DELETE_WINDOW", false );

	XTextProperty windowName;
 	XSizeHints* size_hints = XAllocSizeHints();
 	XWMHints*   wm_hints = XAllocWMHints();
 	XClassHint* class_hints = XAllocClassHint();

	switch ( type )
	{
	case APPLICATION:
	case DOCUMENT:
	case FLOATING:
		//size_hints->flags = PPosition | PSize | PMinSize;
		size_hints->flags = PMinSize;
		size_hints->min_width = 10;
		size_hints->min_height = 10;
		XSetWMProtocols( dpy, this->winId, &atom, 1 );
		break;
	case DIALOG:
	case FIXED_DIALOG:
		//size_hints->flags = PPosition | PSize | PMinSize;
		size_hints->flags = PMinSize | PSize;// | PMaxSize;
		size_hints->min_width  = 100;
		size_hints->min_height = 50;
		size_hints->width      = 800;
		size_hints->height     = 600;
//		size_hints->max_width  = 800;
//		size_hints->max_height = 600;
		XSetWMProtocols( dpy, this->winId, &atom, 1 );
		break;
	}
	
	XStringListToTextProperty( &window_name, 1, &windowName );
	wm_hints->initial_state = NormalState;
	wm_hints->input = True;
	wm_hints->flags = StateHint | InputHint;   
  
	class_hints->res_name = "ISLABS";
	class_hints->res_class = "OpenOCL";

	XSetWMProperties( dpy, this->winId, &windowName, null, null, 0,
	                  size_hints, wm_hints, class_hints );
	XFree( windowName.value );
    
	XFree( size_hints );
	XFree( wm_hints );
	XFree( class_hints );

	delete window_name;
}

/*
void
XPanel::requestXWindowGeometry( int anX,
                                int aY,
                                unsigned int aWidth,
                                unsigned int aHeight,
                                unsigned int aBorderWidth )
{
	if (aWidth  < 0) aWidth = 1;
	if (aHeight < 0) aHeight = 1;

#ifdef OPENOCL_UI_X11_XPANEL_DEBUG
	fprintf( stderr, "%s->requestXWindowGeometry( %i, %i, %i, %i, %i )\n",
			this->getName().getChars(), anX, aY, aWidth, aHeight, aBorderWidth );
#endif

	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XMoveResizeWindow( dpy, this->winId, anX, aY, aWidth, aHeight );
	XSetWindowBorderWidth( dpy, this->winId, aBorderWidth );
	XFlush( dpy );
}
*/

void
XPanel::requestXWindowPosition( int x, int y )
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XMoveWindow( dpy, this->winId, x, y );
	XFlush( dpy );
}

void
XPanel::requestXWindowSize( unsigned int width, unsigned int height )
{
	if (width  <= 0) width  = 1;
	if (height <= 0) height = 1;

	PanelGeometry& outer = this->getOuterGeometry();
	outer.width = width;
	outer.height = height;

	Display* dpy = (Display*) this->xscreen.getXDisplay();

	XResizeWindow( dpy, this->winId, width, height );

	if ( this->fixedSize )
	{
 		XSizeHints* size_hints = XAllocSizeHints();
 
		size_hints->flags = PMinSize | PMaxSize;
		size_hints->min_width  = width;
		size_hints->min_height = height;
		size_hints->max_width  = width;
		size_hints->max_height = height;
		XSetWMProperties( dpy, this->winId, null, null, null, 0, size_hints, null, null );
    		XFree( size_hints );
	}
	XFlush( dpy );
} 

void
XPanel::requestXWindowBorder( unsigned int border )
{
	Display* dpy = (Display*) this->xscreen.getXDisplay();
	XSetWindowBorderWidth( dpy, this->winId, border );
	XFlush( dpy );
}

/*
void
XPanel::setGeometry( int anX,
                     int aY,  
                     unsigned int aWidth,  
                     unsigned int aHeight,  
                     unsigned int aBorderWidth )
{
	IO::err().printf( "%s.XPanel::setGeometry( %i, %i, %u, %u )\n", this->getName().getChars(), anX, aY, aWidth, aHeight );

	//	Is this dead?
	
	abort();

	//this->geometry.x = anX;
	//this->geometry.y = aY;
	//this->geometry.width = aWidth;
	//this->geometry.height = aHeight;
	//this->geometry.borderWidth = aBorderWidth;
}
*/

unsigned long int
XPanel::getWinId() const
{
  return this->winId;
}

void
XPanel::calculateAbsoluteCoordinatesOf( PointerEvent& pe ) const
{
	int panel_x = this->getAbsoluteX();
	int panel_y = this->getAbsoluteY();
	int panel_relative_x = pe.getX();
	int panel_relative_y = pe.getY();
	int event_absolute_x = panel_x + panel_relative_x;
	int event_absolute_y = panel_y + panel_relative_y;

	pe.setAbsoluteCoordinates( event_absolute_x, event_absolute_y );
}




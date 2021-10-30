/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Panel.h"
#include "openocl/ui/PanelGeometry.h"

#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/ExposeEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/base/event/RepositionEvent.h>
#include <openocl/base/event/ResizeEvent.h>
#include <openocl/threads/Mutex.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::threads;
using namespace openocl::ui;

Panel::Panel()
{
	this->outer = new PanelGeometry();
	this->inner = new PanelGeometry();
	this->status = 0;
	this->refreshCountMutex = new Mutex();
	this->refreshCount = 0;
	this->isWithinDispatchEvent = false;
	this->continueClose = false;
	this->closeBoolean = null;
}

Panel::~Panel()
{
	delete this->refreshCountMutex;
	delete this->outer;
	delete this->inner;
}

void
Panel::dispatchEvent( Event* anEvent )
{
	this->isWithinDispatchEvent = true;
	flags type = anEvent->getType();

#ifdef DEBUG_MEMORY
		fprintf( stderr, "Panel::dispatchEvent: " );
		Runtime::printMemorySummary( stderr );
#endif

#ifdef DEBUG_OPENOCL_UI_PANEL
	fprintf( stderr, "%s( %i ).Panel::dispatchEvent: entering\n", this->getName().getChars(), this->getId() );
#endif

	GraphicsContext& gc = this->getGraphicsContext();

	switch ( type )
	{
	case Event::REPOSITION_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: REPOSITION_EVENT\n" );
				#endif
			const RepositionEvent* re = dynamic_cast<const RepositionEvent*>( anEvent );
			this->repositioned( *re );
		}
		break;
	case Event::RESIZE_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: RESIZE_EVENT\n" );
				#endif
			const ResizeEvent* re = dynamic_cast<const ResizeEvent*>( anEvent );
			this->resized( *re );
		}
		//	Intentional fall-through to RELAYOUT_EVENT
	case Event::REFRESH_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: REFRESH_EVENT\n" );
				#endif
			this->checkRefresh();
		}
		//	Intentional fall-through to RELAYOUT_EVENT
	case Event::RELAYOUT_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: RELAYOUT_EVENT\n" );
				#endif
			this->doLayout( gc );
		}
		//	Intentional fall-through to REDRAW_EVENT
	case Event::REDRAW_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: REDRAW_EVENT\n" );
				#endif
			this->redraw( gc );
		}
		break;
		//	Cannot fall-through to expose as Windows requires a valid expose event
		//	to paint to.
	case Event::EXPOSE_EVENT:
		{
				#ifdef DEBUG_OPENOCL_UI_PANEL
				fprintf( stderr, "Panel::dispatchEvent: EXPOSE_EVENT\n" );
				#endif
			this->paint( gc );
		}
		break;
	case Event::CLOSE_EVENT:
		{
			//	Sends a notification ActionEvent up and down the component
			//	hierarchy telling components that this Panel is about to
			//	be closed.

			this->beginClose();
			this->forwardEvent( *anEvent );
			this->finishClose();
		}
		break;
	case Event::FOCUS_EVENT:
				#ifdef DEBUG_OPENOCL_UI_PANEL
				#endif
		this->Container::forwardEvent( *anEvent );
		break;
	case Event::CHANGE_EVENT:
				#ifdef DEBUG_OPENOCL_UI_PANEL
				#endif
		this->forwardRedrawAllChangeEvent();
		break;
	default:
		switch ( Event::POINTER_EVENT & type )
		{
		case Event::POINTER_EVENT:
			#ifdef DEBUG_OPENOCL_UI_PANEL
			fprintf( stderr, "Panel::dispatchEvent: POINTER_EVENT\n" );
			#endif
			PointerEvent* pe = dynamic_cast<PointerEvent*>( anEvent );
			if ( pe )
			{
				//fprintf( stderr, "Panel::dispatchEvent: %3i:%3i\n", pe->getX(), pe->getY() );
			
				this->calculateAbsoluteCoordinatesOf( *pe );
			}
			#ifdef DEBUG_OPENOCL_UI_PANEL_DISPATCHEVENT
			if ( Event::MOUSE_EVENT == type )
			{
				fprintf( stderr, "Panel::dispatchEvent( MOUSE_EVENT )\n" );
			}
			#endif
		}
		
#ifdef DEBUG_OPENOCL_UI_PANEL
		switch ( Event::KEYDOWN_EVENT & type )
		{
		case Event::KEYDOWN_EVENT:
			KeyEvent* ke = dynamic_cast<KeyEvent*>( anEvent );

			int key = ke->getKey();
			int mod = ke->getModifiers();
			fprintf( stderr, "Panel::dispatchEvent: CODE: %u MOD: %0x%x\n", key, mod );
		}
#endif
		
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
	this->flush();
	delete anEvent;

#ifdef DEBUG_OPENOCL_UI_PANEL
	fprintf( stderr, "Panel::dispatchEvent: leaving\n" );
#endif
#ifdef DEBUG_MEMORY
	fprintf( stderr, "Panel::dispatchEvent: <<< objCount: %lli\n", Object::getObjectCount() );
#endif
	this->isWithinDispatchEvent = false;
}

bool
Panel::repositioned( const RepositionEvent& aRepositionEvent )
{
	bool changed = true;

	//	Should work but in X11 can generate strange events.
	//
	//this->outer->x = aRepositionEvent.getX();
	//this->outer->y = aRepositionEvent.getY();

	//	Works but is slow.
	//*this->outer = queryOuterGeometry();
	*this->inner = queryInnerGeometry();

	//	This should work. But does require an X call each
	//	time the window is updated. Perhaps a kludge can
	//	only do this when necessary (when window initally
	//	appears).

	Coordinates coords = this->queryOuterCoordinates();
	this->outer->x = coords.x;
	this->outer->y = coords.y;

	return changed;
}

bool
Panel::resized( const openocl::base::event::ResizeEvent& aResizeEvent )
{
	//unsigned int new_width  = aResizeEvent.getWidth();
	//unsigned int new_height = aResizeEvent.getHeight();

	bool changed = true; //((this->outer->width != new_width) || (this->outer->height != new_height));

	if ( changed )
	{		
		this->outer->width  = aResizeEvent.getWidth();
		this->outer->height = aResizeEvent.getHeight();

		this->inner->width  = this->outer->width  - (2 * this->outer->border);
		this->inner->height = this->outer->height - (2 * this->outer->border);

		PanelGeometry inner = *this->inner;

		//	We directly call the superclass Component::setBounds(...) method
		//	as this will bypass resizing of the panel by the window manager.
		this->Component::setBounds( 0, 0, inner.width, inner.height );
		//   this->Container::setBounds( inner.x, inner.y, inner.width, inner.height );
	}

	return changed;
}

void
Panel::requestRefresh()
{
	this->refreshCountMutex->lock( "Panel::requestRefresh()" );
	{
		this->refreshCount++;
		//fprintf( stderr, "Panel::requestRefresh(): %i\n", this->refreshCount );
	}
	this->refreshCountMutex->unlock( "Panel::requestRefresh()" );

	//this->refresh();
	
	//	This should be the only place where update() is called other
	//	than in dispatchEvent. It is needed here because requestRefresh()
	//	is called by models
	
	this->sendRefreshMessage();
}

void
Panel::redrawAll()
{
	fprintf( stderr, "Panel::redrawAll()\n" );
	this->forwardRedrawAllChangeEvent();
	this->relayout();
}

void
Panel::setContinueClose( bool continueClose )
{
	this->continueClose = continueClose;
}

void
Panel::setCloseBoolean( bool* loop )
{
	this->closeBoolean = loop;
}

//------------------------------------------------------------------------
//	public pseudo-const methods
//------------------------------------------------------------------------

Coordinates
Panel::getOuterCoordinates() const
{
	Coordinates c;

	try
	{
		const Panel& parent_panel = this->getContainingPanel();
		c = parent_panel.getOuterCoordinates();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	c.x += this->outer->x;
	c.y += this->outer->y;

	return c;
}	

Coordinates
Panel::getOffsetCoordinates() const
{
	Coordinates c( this->inner->x, this->inner->y );
	return c;
}

PanelGeometry&
Panel::getOuterGeometry()
{
	return *this->outer;
}

PanelGeometry&
Panel::getInnerGeometry()
{
	return *this->inner;
}

void
Panel::beginClose()
{
	ActionEvent close_request_to_application( *this, 0, "CLOSE_FRAME_REQUEST.openocl::ui::Panel" );
	this->continueClose = true;
	//	If the application doesn't want the frame to close it will
	//	set continueClose to false.
	this->fireEvent( close_request_to_application );
}

void
Panel::finishClose()
{
	if ( this->continueClose )
	{
		#ifdef DEBUG_OPENOCL_UI_PANEL
		fprintf( stderr, "%s.Panel::finishClose(): sending close panel action event\n", this->getName().getChars() );
		#endif

		ActionEvent close_event( *this, this->getId(), "CLOSE_FRAME.openocl::ui::Panel" );
		this->show( Panel::HIDDEN );
		this->fireEvent( close_event );

		//	Let owner of panel know that panel is closed
		//	in most cases will cause the exit out of
		//	event loop.
		if ( this->closeBoolean ) *this->closeBoolean = false;
	}
}

//void
//Panel::refresh() const
//{
//	//const_cast<Panel*>( this )->status |= Panel::REFRESH;
//}

void
Panel::relayout() const
{
	const_cast<Panel*>( this )->status |= Panel::RELAYOUT;
	if ( ! this->isWithinDispatchEvent ) this->update();
}

void
Panel::redraw() const
{
	const_cast<Panel*>( this )->status |= Panel::REDRAW;
	if ( ! this->isWithinDispatchEvent ) this->update();
}

void
Panel::repaint() const
{
	const_cast<Panel*>( this )->status |= Panel::REPAINT;
	if ( ! this->isWithinDispatchEvent ) this->update();
}

void
Panel::update() const
{
	const_cast<Panel*>( this )->update();
}

const PanelGeometry&
Panel::getOuterGeometry() const
{
	return *this->outer;
}

const PanelGeometry&
Panel::getInnerGeometry() const
{
	return *this->inner;
}

void
Panel::checkRefresh()
{
	//	Models are able to request a refresh of the UI by calling request refresh.
	//	This increments a refresh counter, then calls refresh, which sends a custom
	//	refresh event to the window server, which is then forwarded to Panel::dispatach
	//	event.
	//
	//	It may also be the case that a Control such as FileBox will need to update itself
	//	due to an ActionEvent from one of its components. In this situation it will also
	//	call requestRefresh. As there is no way to customize the refresh action event sent
	//	such components that use requestRefresh in this way should keep a boolean value that
	//	also is used to indicate whether they should refresh.
	//
	//	This method is called from Panel::dispatchEvent, when a dispatched RefreshEvent is
	//	encountered.
	//	This method reduces the refresh count then creates an REFRESH action event which is
	//	forwarded to contained containers and components.
	//	When used as part of MVC, the ActionEvent will be encountered by a Control, which will
	//	then fire an event to its View, causing it to check its event queue.

	//fprintf( stderr, "Panel::checkRefresh: %i\n", this->refreshCount );

	bool do_refresh = true;
	this->refreshCountMutex->lock( "Panel::checkRefresh()" );
	{
		if ( 0 < this->refreshCount )
		{
			do_refresh = true;
			//this->refreshCount = 0;
			this->refreshCount--;
		}
	}
	this->refreshCountMutex->unlock( "Panel::checkRefresh()" );

	if ( do_refresh )
	{
		ActionEvent ae( *this, this->getId(), "REFRESH.openocl::ui::Panel.CHECK_EVENTQUEUE" );
		this->forwardEvent( ae );
		//this->forwardChangeEvent();
	}
}

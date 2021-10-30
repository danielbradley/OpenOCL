/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/mvc.h"
#include "openocl/mvc/Model.h"
#include "openocl/mvc/View.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/RefreshEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/Debug.h>
#include <openocl/mvc/DocumentPaneMonitor.h>
#include <openocl/threads/EventQueue.h>
#include <openocl/threads/Mutex.h>
#include <openocl/ui/Blank.h>
#include <openocl/ui/Component.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/DocumentPane.h>
#include <openocl/ui/Panel.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::mvc;
using namespace openocl::threads;
using namespace openocl::ui;
using namespace openocl::util;

View::View( Model& aModel )
: Object(), model( aModel )
{
	this->documentPane     = new DocumentPane();// new DocumentPaneMonitor();
	this->eventListeners   = new Sequence();
	this->documentPane->addEventListener( *this );
	this->model.addChangeEventListener( *this );
	this->changeEvents = new EventQueue();
}

View::~View()
{
	this->model.removeChangeEventListener( *this );
	this->documentPane->removeEventListener( *this );
	delete this->eventListeners;
	delete this->documentPane;
	delete this->changeEvents;
}

//------------------------------------------------------------------------------
//	public virtual methods (EventListener)
//------------------------------------------------------------------------------

/**
 *  The view is a listener of component hierarchy.
 *
 *  Subclasses that want to receive Action Events should override
 *  this method.
 */
void
View::deliverEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	
	if ( type == Event::ACTION_EVENT )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& command = ae.getCommand();
		
#ifdef DEBUG_OPENOCL_MVC_VIEW
		fprintf( stderr, "View::deliverEvent: command=%s\n", command.getChars() );
#endif
		
		if ( command.equals( "REFRESH.openocl::ui::Panel.CHECK_EVENTQUEUE" ) )
		{
#ifdef DEBUG_OPENOCL_MVC_VIEW
			fprintf( stderr, "View::deliverEvent(): refreshing control\n" );
#endif
			//XXX-Debug
			this->refresh();
		}
		else if ( command.startsWith( "RUNTIMEEXCEPTION." ) )
		{
			fprintf( stderr, "View::deliverEvent: caught: RuntimeException: %s\n", ae.getData().getChars() );
			this->fireEvent( anEvent );
		}
		else
		{
			ActionEvent* ae_copy = new ActionEvent( ae );
			this->model.getEventQueue().postEvent( ae_copy );
			this->fireEvent( anEvent );
#ifdef DEBUG_OPENOCL_MVC_VIEW
			fprintf( stderr, "View::deliverEvent(): posted event\n" );
#endif
		}
	}
#ifdef DEBUG_OPENOCL_MVC_VIEW
	else if ( Event::CHANGE_EVENT == type )
	{
		//	ChangeEvents are prograted from a changed visual element back
		//	through its parents so that they can set themselves to be redrawn.
		//	View is not a visual element, but as it registers itself as
		//	a listener for action events it also gets ChangeEvents.

		fprintf( stderr, "View::deliverEvent(): ChangeEvent\n" );
	}
	else
	{
		fprintf( stderr, "View::deliverEvent(): WARNING NON-ACTIONEVENT!!\n" );
		fprintf( stderr, "message=0x%x\n", anEvent.getType() );
		fprintf( stderr, "View::deliverEvent(): WARNING NON-ACTIONEVENT!!\n" );
	}
#endif
}

//------------------------------------------------------------------------------
//	public virtual methods (ChangeEventListener)
//------------------------------------------------------------------------------

/**
 *  The view is a listener of the Model, therefore it will be
 *  receiving Change Events through this method.
 *
 *  ??Subclasses that want to receive Change Events should override
 *  ??this method.
 *
 *  This method is called by Model.fireChangeEvent, therefore it is within a
 *  non-gui thread. The event is first added to a thread-safe event queue,
 *  then the requestRefresh event is called.
 */
void
View::deliverChangeEvent( RefreshEvent* aRefreshEvent )
{
#ifdef DEBUG_OPENOCL_MVC_VIEW
	fprintf( stderr, "View::deliverChangeEvent()\n" );
#endif
	const Object& source     = aRefreshEvent->getEventSource();
	unsigned long int target = aRefreshEvent->getTargetId();
	const String& objectId   = aRefreshEvent->getObjectId();

#ifndef DEBUG_OPENOCL_MVC_DISABLE_REFRESHEVENTS
	ActionEvent* actionEvent = new ActionEvent( source, target, "REFRESH.openocl::mvc::View", objectId );
	this->changeEvents->postEvent( actionEvent );
	this->requestRefresh();
#endif

	delete aRefreshEvent;
}

/**
 *  View is a component (EventHandler) of the model, which will
 *  be propagating Input Events to the view, using this method.
 *
 *  Subclasses that want to receive Input Events should override
 *  this method.
 *
 *  REMOVED
 *
 *  Actually a views components will receive their own events
 *  through the dispatch mechanism.
 *
 * void
 * View::dispatchEvent( Event* anEvent )
 * {
 *   Debug::entering( "View", "dispatchEvent", "anEvent" );
 *   delete anEvent;
 *   Debug::leaving();
 * }
 */

void
View::deliverRefreshEvent( const Event& anEvent )
{
	this->refresh();
}

//------------------------------------------------------------------------------
//	public virtual methods (EventHandler)
//------------------------------------------------------------------------------

// EventRelay
void
View::addEventListener( EventListener& anEventListener )
{
	Object* obj = dynamic_cast<Object*>( &anEventListener );
	this->eventListeners->add( obj );
}

void
View::removeEventListener( EventListener& anEventListener )
{
	unsigned int max = this->eventListeners->getLength();
	Object& object = dynamic_cast<Object&>( anEventListener );
	Object* obj = &object;
	void* el = (void*) obj;

	for ( unsigned int i=0; i < max; i++ )
	{
		void* obj = (void*) &this->eventListeners->elementAt( i );
		if ( obj == el )
		{
			this->eventListeners->remove( i );
			this->removeEventListener( anEventListener );
			break;
		}
	}
}

//------------------------------------------------------------------------------
//	public virtual methods (View)
//------------------------------------------------------------------------------

/*
 *  The requestRefresh method calls the requestRefresh method on the Panel
 *  associated with this view. This increments a refreshCount member which
 *  checked by the gui thread during each loop.
 *
 *  If it is greater than zero a refresh ActionEvent is forward through the
 *  component hierarchy and is intercepted by the first control encountered,
 *  which then calls fireEvent to pass it to the appropriate View, which
 *  then calls refresh.
 */
void
View::requestRefresh()
{
	//	Called by Model to cause View to refresh itself
	//	by calling Panel.refresh(), which sends a message
	//	to Window Server, which causes a ResizeEvent to be
	//	sent to all panels.
	//
	//	Is this thread-safe? I think Panel.refresh() is fairly
	//	thread safe as it only access data members that should'nt
	//	change.
	//
	//	Is this->documentPane->getContainingPanel() thread-safe.
	
	try
	{
		this->documentPane->getContainingPanel().requestRefresh();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	} catch ( ... ) {
		fprintf( stderr, "View::refresh: unexpected exception!! aborting!\n" );
		abort();
	}
}

//------------------------------------------------------------------------------
//	public methods (View)
//------------------------------------------------------------------------------

void
View::addViewComponent( Component& viewComponent )
{
	this->documentPane->add( viewComponent );
}

void
View::removeViewComponent( Component& viewComponent )
{
	this->documentPane->remove( viewComponent );
}

Component&
View::getComponent()
{
	return *this->documentPane;
}

//------------------------------------------------------------------------------
//	protected virtual methods (EventHandler)
//------------------------------------------------------------------------------

// EventRelay
void
View::fireEvent( const Event& anEvent )
{
	Iterator* it = this->eventListeners->iterator();
	while ( it->hasNext() )
	{
		Object& obj = (Object&) it->next();
		EventListener* eventListener = dynamic_cast<EventListener*>( &obj );
		eventListener->deliverEvent( anEvent );
	}
	delete it;
}

//------------------------------------------------------------------------------
//	private methods
//------------------------------------------------------------------------------

void
View::refresh()
{
	fprintf( stderr, "View::refresh()\n" );
	switch ( 2 )
	{
	case 0:
		{
			unsigned int len;
			while ( 0 < (len = this->changeEvents->getLength()) )
			{
				for ( unsigned int i=0; i < len; i++ )
				{
					ActionEvent* ae = this->changeEvents->nextEvent();
					const String& command = ae->getCommand();
					if ( command.startsWith( "REFRESH." ) )
					{
						const String& changed_object_id = ae->getData();
						//fprintf( stderr, "View::refresh: %s\n", changed_object_id.getChars() );
						this->refresh( changed_object_id );
					}
					delete ae;
				}
			}
		}
		break;
	case 1:
		{
			unsigned int len;
			while ( 0 < (len = this->changeEvents->getLength()) )
			{
				//delete this->changeEvents->nextEvent();
				ActionEvent* ae = this->changeEvents->nextEvent();
				const String& command = ae->getCommand();
				if ( command.startsWith( "REFRESH." ) )
				{
					const String& changed_object_id = ae->getData();
					//fprintf( stderr, "View::refresh: %s\n", changed_object_id.getChars() );
					this->refresh( changed_object_id );
					this->documentPane->getContainingPanel().redrawAll();
				}
				delete ae;
			}
		}
		break;
	case 2:
		{
			unsigned int len;
			if ( 0 < (len = this->changeEvents->getLength()) )
			{
				//delete this->changeEvents->nextEvent();
				ActionEvent* ae = this->changeEvents->nextEvent();
				const String& command = ae->getCommand();
				if ( command.startsWith( "REFRESH." ) )
				{
					const String& changed_object_id = ae->getData();
					this->refresh( changed_object_id );
				}
				delete ae;

				//if ( 1 < len )
				//{
				//	this->documentPane->getContainingPanel().relayout();
				//}
			}
		}
	}

/*
*/
}

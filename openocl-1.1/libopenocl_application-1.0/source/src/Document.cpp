/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/application/Application.h"
#include "openocl/application/Document.h"

#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/io/Path.h>
#include <openocl/mvc/View.h>
#include <openocl/threads/EventQueue.h>
#include <openocl/threads/Mutex.h>
#include <openocl/ui/Component.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/Panel.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::threads;
using namespace openocl::mvc;
using namespace openocl::ui;
using namespace openocl::util;

Document::Document()
{
	this->defaultViewComponent        = new Container();
	this->eventQueue                  = new EventQueue();
	
	this->documentAlias               = new String();
	this->documentKey                 = new String();
	this->documentLocation            = new String();
	this->provisionalDocumentLocation = new String();
	this->toolbars                    = new Sequence();
	this->auxViews                    = new Sequence();

	this->view        = null;
	this->visual      = null;
	this->application = null;
	this->changed     = false;
}

Document::~Document()
{
	delete this->auxViews;
	delete this->toolbars;

	delete this->provisionalDocumentLocation;
	delete this->documentLocation;
	delete this->documentKey;
	delete this->documentAlias;
	delete this->eventQueue;
	delete this->defaultViewComponent;
}

//-----------------------------------------------------------------
//	public virtual methods (EventListener)
//-----------------------------------------------------------------

void
Document::deliverEvent( const Event& anEvent )
{
	unsigned int event_type = anEvent.getType();

	if ( Event::ACTION_EVENT == (event_type & Event::ACTION_EVENT) )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		
		if ( ae.getCommand().startsWith( "CLOSE.openocl::ui::Panel" ) )
		{
			this->handleCloseEvent( ae );
		}
		else if ( ae.getCommand().startsWith( "RUNTIMEEXCEPTION." ) )
		{
			//	For some reason that I can't fathom this causes GetExitCodeThread to
			//	hang.
			//ActionEvent close_event( *this, 0, "CLOSE.openocl::application::Document", this->getDocumentName() );
			//this->application->deliverEvent( close_event );
		}
		else if ( !this->handleActionEvent( ae ) && this->application )
		{
			this->application->deliverEvent( ae );
		}
	}
}

//-----------------------------------------------------------------
//	public virtual methods (Document) dummy implementations
//-----------------------------------------------------------------

bool
Document::handleActionEvent( const ActionEvent& anEvent )
{
	return true;
}

void
Document::saveDocument( int typeId )
{}

void
Document::exportDocument( const String& location, int typeId )
{}

//-----------------------------------------------------------------
//	public virtual methods (Document)
//-----------------------------------------------------------------

void
Document::forwardEvent( const Event& anEvent )
{
#ifdef DEBUG_OPENOCL_APPLICATION_DOCUMENT
	fprintf( stderr, "Document::forwardEvent()\n" );
#endif

	unsigned int event_type = anEvent.getType();

	if ( Event::ACTION_EVENT == (event_type & Event::ACTION_EVENT) )
	{
		ActionEvent* ae = dynamic_cast<ActionEvent*>( anEvent.copy() );
		
		if ( this->handleActionEvent( *ae ) )
		{
			delete ae;
		} else {
#ifdef DEBUG_OPENOCL_APPLICATION_DOCUMENT
			fprintf( stderr, "Document::forwardEvent(): appending event to queue\n" );
#endif
			this->eventQueue->postEvent( ae );
		}
	} else {
		fprintf( stderr, "Document::forwardEvent(): aborting; unexpected non-ActionEvent!!\n" );
		abort();
	}
}

void
Document::saveDocument()
{
	this->saveDocument( 0 );
}

//-----------------------------------------------------------------
//	public methods (Document)
//-----------------------------------------------------------------

void
Document::addAuxiliaryView( openocl::mvc::View& anAuxView )
{
	this->auxViews->add( anAuxView );
}

void
Document::setApplication( openocl::application::Application& anApplication )
{
	this->application = &anApplication;
}

void
Document::setChanged( bool changed )
{
	this->changed = changed;
}

void
Document::setDocumentAlias( const String& documentAlias )
{
	Component& view_component = this->getViewComponent();
	view_component.setName( documentAlias );
	delete this->documentAlias;
	this->documentAlias = new String( documentAlias );
}

void
Document::setDocumentKey( const String& documentKey )
{
	delete this->documentKey;
	this->documentKey = new String( documentKey );
}

void
Document::setDocumentLocation( const String& documentLocation )
{
	//	Whenever the document location is set, the alias is set
	//	to the filename of the location.
	
	Path path( documentLocation );
	this->setDocumentAlias( path.getBasename() );
	delete this->documentLocation;
	this->documentLocation = new String( documentLocation );
	this->setChanged( true );
}

void
Document::setProvisionalDocumentLocation( const String& provisionalDocumentLocation )
{
	//	Whenever the provisional name is set the documentName is
	//	reset to the empty string.
	delete this->documentLocation;
	this->documentLocation = new String();

	Path path( provisionalDocumentLocation );
	this->setDocumentAlias( path.getBasename() );
	delete this->provisionalDocumentLocation;
	this->provisionalDocumentLocation = new String( provisionalDocumentLocation );
	this->setChanged( true );
}

void
Document::setView( openocl::mvc::View& aView )
{
	this->view = &aView;
}

void
Document::setVisual( openocl::ui::Visual& aVisual )
{
	this->visual = &aVisual;
}

void
Document::stop()
{
	ActionEvent* ae = new ActionEvent( *this, 0, "STOP.openocl::application::Document.STOPPED", "STOP" );
	this->forwardEvent( *ae );
	delete ae;
	//this->eventQueue->postEvent( ae );
}

Component&
Document::getViewComponent()
{
	if ( this->view )
	{
		return this->view->getComponent();
	} else {
		return *this->defaultViewComponent;
	}
}

EventQueue&
Document::getEventQueue()
{
	return *this->eventQueue;
}

Visual&
Document::getVisual()
{
	return *this->visual;
}

Application&
Document::getApplication()
{
	return *this->application;
}

openocl::util::Sequence&
Document::getToolbars()
{
	return *this->toolbars;
}

//-----------------------------------------------------------------
//	public virtual constant methods (Document)
//-----------------------------------------------------------------

const String&
Document::getDisplayName() const
{
	return *this->documentAlias;
}

const String&
Document::getDocumentAlias() const
{
	return *this->documentAlias;
}

const String&
Document::getDocumentKey() const
{
	return *this->documentKey;
}

const String&
Document::getDocumentLocation() const
{
	return *this->documentLocation;
}

const String&
Document::getProvisionalDocumentLocation() const
{
	return *this->provisionalDocumentLocation;
}

bool
Document::hasChanged() const
{
	return this->changed;
}

bool
Document::hasName() const
{
	return (bool) this->documentLocation->getLength();
}

//-----------------------------------------------------------------
//	public constant methods (Document)
//-----------------------------------------------------------------

const Component&
Document::getViewComponent() const
{
	if ( this->view )
	{
		return this->view->getComponent();
	} else {
		return *this->defaultViewComponent;
	}
}

//-----------------------------------------------------------------
//	virtual protected methods (Document)
//
//	These methods should only be called from the overridden
//	run method.
//-----------------------------------------------------------------

bool
Document::run()
{
	return false;

	//	//	Example event handler (non-optimized)
	//
	//	EventQueue& event_queue = this->getEventQueue();
	//
	//	bool loop = true;
	//	while ( loop )
	//	{
	//		ActionEvent* actionEvent = event_queue.nextEvent();
	//		const String& command = actionEvent->getCommand();
	//		if ( command.startsWith( "STOP." ) )
	//		{
	//			loop = false;
	//		}
	//		delete actionEvent;
	//	}
	//	return true;
}

void
Document::runSaveDocument( int typeId )
{}

void
Document::runExportDocument( const String& location, int typeId )
{}


//-----------------------------------------------------------------
//	private methods (Document)
//-----------------------------------------------------------------

void
Document::handleCloseEvent( const ActionEvent& ae )
{
	//	XXX Extremely dodgy
	//
	//	In effect what we are doing here is asking Application
	//	to kill this document before the call actually returns.
	//	So when "this->application->deliverEvent( ae )" returns
	//	"this" will no longer be valid.
	//
	//	While this will "work"; invalidating "this" just seems
	//	wrong. In the future it will be better to send a custom
	//	message to the EventManager so that it will close the
	//	right document

	unsigned long win_id = ae.getTargetId();
	
	if ( this->view->getComponent().getContainingPanel().getId() == win_id )
	{
		ActionEvent close_event( *this, win_id, "CLOSE.openocl::application::Document", this->getDocumentLocation() );
		this->application->deliverEvent( close_event );
	} else {
		unsigned int max = this->auxViews->getLength();
		for ( unsigned int i=0; i < max; i++ )
		{
			View& view = dynamic_cast<View&>( this->auxViews->elementAt( i ) );
			Component& com = view.getComponent();
			Panel& panel = com.getContainingPanel();
			if ( panel.getId() == win_id )
			{
				panel.show( Panel::HIDDEN );
			}
			break;
		}
		//	XX  This needs to be changed.
		//ActionEvent close_event( *this, win_id, "openocl::application::Document::CLOSE", this->getDocumentName() );
		//this->application->deliverEvent( ae );
	}
}



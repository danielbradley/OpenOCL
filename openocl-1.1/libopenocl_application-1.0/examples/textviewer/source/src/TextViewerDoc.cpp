#include "openocl/textviewer/TextViewerDoc.h"
#include "openocl/textviewer/TextViewerModel.h"
#include "openocl/textviewer/TextViewerView.h"

#include <openocl/base/String.h>
#include <openocl/base/User.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/threads/EventQueue.h>
#include <openocl/io/Path.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::threads;
using namespace openocl::textviewer;

TextViewerDoc::TextViewerDoc()
{
	this->model = new TextViewerModel();
	this->view = new TextViewerView( *this->model );
	this->view->addEventListener( *this );
	this->setView( *this->view );
}

TextViewerDoc::~TextViewerDoc()
{
	this->view->removeEventListener( *this );
	delete this->view;
	delete this->model;
}

//-----------------------------------------------------------------
//	public virtual methods (Document)
//-----------------------------------------------------------------

void
TextViewerDoc::init( const Values& values )
{}

void
TextViewerDoc::saveDocument() {}

void
TextViewerDoc::saveDocument( int typeId ) {}

void
TextViewerDoc::exportDocument( const openocl::base::String& location, int typeId ) {}

bool
TextViewerDoc::handleActionEvent( const openocl::base::event::ActionEvent& ae )
{
	const String& command = ae.getCommand();
	const String& data = ae.getData();
	//fprintf( stderr, "WaterfallDoc::handleActionEvent: %s -> %s\n", command.getChars(), data.getChars() );
	
	if ( command.startsWith( "CHANGE_DIR." ) )
	{
		this->setDocumentLocation( data );
	}

	return false;
}

bool
TextViewerDoc::run()
{
	EventQueue& eventQueue = this->getEventQueue();

	this->load();

	bool loop = true;
	while ( loop )
	{
		ActionEvent* actionEvent = eventQueue.nextEvent();
		const String& command = actionEvent->getCommand();
		
		if ( command.startsWith( "LOAD." ) )
		{
			delete actionEvent;
			this->model->load( this->getDocumentLocation() );
		}
		else if ( command.startsWith( "STOP.openocl::application::Document" ) )
		{
			delete actionEvent;
			loop = false;
		}
	}
	
	return true;
}

//
//	public methods
//

void
TextViewerDoc::load()
{
	this->getEventQueue().postEvent( new ActionEvent( *this, 0, "LOAD.TextViewerDoc" ) );
}

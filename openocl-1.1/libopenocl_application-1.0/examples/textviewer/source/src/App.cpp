#include "App.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/controls/FileChooser.h>
#include <openocl/io/File.h>
#include <openocl/io/Path.h>
#include <openocl/ui/ApplicationFrame.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/Menu.h>
#include <openocl/ui/MenuItem.h>
#include <openocl/util/Arguments.h>
#include <openocl/util/Properties.h>

#include <openocl/textviewer/TextViewerDoc.h>

#include <cstdio>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

using namespace openocl::textviewer;

App::App( int argc, char** argv ) : Application()
{
	Arguments args( argc, argv );
	this->arguments = args.parse();
	
	this->setApplicationTitle( "Text Viewer" );

	{
		this->menuFile = new Menu( "File", "FILE", "File" );
		this->menuFile->addMenuItem( new MenuItem( "Open MenuItem", "OPEN", "Open" ) );
		this->menuFile->addMenuItem( new MenuItem( "Close MenuItem", "CLOSE", "Close" ) );
		this->addApplicationMenuItem( "file", *menuFile );
	}
	
	this->parseArguments();
}

App::~App()
{
	this->removeApplicationMenuItem( *this->menuFile );

	delete this->menuFile;

	delete this->arguments;
}

void
App::deliverActionEvent( const ActionEvent& anActionEvent )
{}

void
App::newDocument( const String& name )
{}

void
App::openDocument()
{
	FileChooser chooser;
	chooser.setPrefix( "Open " );
	Panel& panel = this->getApplicationFrame().getTopPanel();

	if ( Dialog::showDialog( panel, chooser ) )
	{
		this->openDocument( chooser.getChosenFile() );
	}
}

void
App::saveDocument()
{}

void
App::saveDocumentAs()
{}

bool
App::closeDocument()
{
	Document* doc = this->removeActiveDocument();
	doc->stop();
	doc->wait();
	delete doc;

	return true;
}

bool
App::closeDocument( const openocl::base::String& documentName )
{
	return true;
}

bool
App::closeAllDocuments()
{
	return true;
}

bool
App::closeApplication()
{
	return true;
}

//--------------------------------------------------------------------
//	private initialization methods
//--------------------------------------------------------------------

void
App::parseArguments()
{
	Iterator* it = this->arguments->propertyNames();
	while ( it->hasNext() )
	{
		const String& key = dynamic_cast<const String&>( it->next() );
		Path path( key );
		File file( path );
		if ( file.exists() )
		{
			const String& location = path.getAbsolute();
			this->openDocument( location );
		}
	}
	delete it;
}

//--------------------------------------------------------------------
//	private methods
//--------------------------------------------------------------------

void
App::openDocument( const String& location )
{
	Document* doc = new TextViewerDoc();
	doc->setDocumentLocation( location );
	this->addDocument( doc );
	doc->start();
}


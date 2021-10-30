/*
 *  Copyright (c) 2005-2006 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/application/Application.h"
#include "openocl/application/Document.h"
#include "openocl/application/DocumentManager.h"
//#include "openocl/application/MacDocManager.h"
#include "openocl/application/TabbedDocumentManager.h"
#include <openocl/base/Exception.h>
#include <openocl/base/FormattedString.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/User.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/controls/WarningBox.h>
#include <openocl/io/Directory.h>
#include <openocl/io/File.h>
#include <openocl/io/FileInputStream.h>
#include <openocl/io/LineParser.h>
#include <openocl/io/Path.h>
#include <openocl/imaging/Color.h>
#include <openocl/ui/ApplicationFrame.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/MenuGroup.h>
#include <openocl/ui/Menu.h>
#include <openocl/ui/MenuItem.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/RuntimeSummaryItem.h>
#include <openocl/ui/TabbedView.h>
#include <openocl/ui/ToolBar.h>
#include <openocl/ui/ToolBarGroup.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Dictionary.h>
#include <openocl/util/Index.h>
#include <openocl/util/Properties.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/StringKey.h>
#include <openocl/util/StringTokenizer.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::io;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int Application::nextDocumentId = 0;

Application::Application( flags type )
{
	this->visual = Visual::getDefaultInstance();
	
	switch ( type )
	{
	//case Application::FDI:
	//	this->documentManager = new MacDocManager( *visual, *this );
	//	break;
	case Application::TABBED:
	default:
		this->documentManager = new TabbedDocumentManager( *visual, *this );
	}
	
	this->init_stage1();
	this->init_stage2();
}

Application::Application( DocumentManager* aDocumentManager )
{
	this->visual = Visual::getDefaultInstance();
	this->documentManager = aDocumentManager;

	this->init_stage1();
	this->init_stage2();
}

void
Application::init_stage1()
{
	this->applicationTitle = new String();

	this->masterMenu   = new Menu( "Master", "MASTER", "Master" );
	this->masterMenu->addMenuItem( new MenuItem( "About", "ABOUT", "About" ) );
	this->masterMenu->addMenuItem( new MenuItem( "Quit", "QUIT", "Quit" ) );
	this->menuGroup    = new MenuGroup( MenuGroup::HORIZONTAL );
	this->menuGroup->setOuterShade( Color::SHADE2 );
	this->menuGroup->add( *this->masterMenu );

	this->applicationFrame = new ApplicationFrame( *this->visual, &this->run );
	//	This should access the last geometry from preferences.
	//this->applicationFrame->requestGeometry( 100, 100, 900, 500, 0 );
	this->applicationFrame->requestSize( 900, 700 );
}

void
Application::init_stage2()
{
	Panel& panel = this->applicationFrame->getTopPanel();
	GraphicsContext& gc = panel.getGraphicsContext();

	panel.getVisual().getOffscreenImageManager().loadDefaultImages( gc );

	//	First set up the menus. For debugging we put this first, as this manipulating
	//	menus is a common thing for subclass constructors to do.

	//	Configuration and preferences
	this->configuration = new Properties();
	this->preferences   = new Properties();
	
	this->menuItems    = new Index();
	this->toolbars     = new Index();
	
	this->toolBarGroup = new ToolBarGroup();

#ifdef OPENOCL_DEBUG_APPLICATION_MEMORY
	this->rsi = new RuntimeSummaryItem();
	panel.add( *this->rsi );
#endif
	panel.add( *this->menuGroup );
	panel.add( *this->toolBarGroup );
	
	try
	{
		panel.add( this->documentManager->getDocumentManagerComponent() );
	} catch ( NoSuchElementException* ex ) {
		//	If an exception is thrown it indicates that the Document Manager 
		//	creates its own frames to display documents.
		delete ex;
	}
	
	this->run = true;

	//	Add listeners
	panel.addEventListener( *this );

}

Application::~Application()
{
	this->forceRemoveDocuments();
	this->finit();

	delete this->documentManager;
	delete this->visual;
}

void
Application::forceRemoveDocuments()
{
	Document* doc = null;
	try
	{
		while ( (doc = this->removeActiveDocument()) )
		{
			doc->stop();
			doc->wait();
			delete doc;
		}
	}
	catch ( NoSuchElementException* ex )
	{
		delete ex;
	}
}

void
Application::finit()
{
	Panel& panel = this->applicationFrame->getTopPanel();

	//	XXX Need to unload default images from Offscreen Image Manager
	panel.removeEventListener( *this );

	panel.remove( this->documentManager->getDocumentManagerComponent() );
	
	panel.remove( *this->toolBarGroup );
	panel.remove( *this->menuGroup );
		
#ifdef OPENOCL_DEBUG_APPLICATION_MEMORY
	panel.remove( *this->rsi );
	delete this->rsi;
#endif
	delete this->toolBarGroup;

	delete this->toolbars;		//	Index
	delete this->menuItems;		//	Index

	delete this->preferences;	//	Properties
	delete this->configuration;	//	Properties

	delete this->applicationFrame;

	this->menuGroup->remove( *this->masterMenu );
	delete this->menuGroup;
	delete this->masterMenu;
	delete this->applicationTitle;
}

//-----------------------------------------------------------------------
//	public virtual methods (EventListener)
//-----------------------------------------------------------------------

void
Application::deliverEvent( const openocl::base::event::Event& anEvent )
{
	int event_type = anEvent.getType();
	switch ( event_type )
	{
	case Event::ACTION_EVENT:
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& command = ae.getCommand();

#ifdef DEBUG_OPENOCL_APPLICATION_APPLICATION
		fprintf( stderr, "Application::deliverEvent: command: %s\n", command.getChars() );
#endif
		try
		{
			if ( command.startsWith( "NEW." ) )
			{
				this->newDocument( ae.getData() );
			}
			else if ( command.startsWith( "OPEN." ) )
			{
				this->openDocument();
			}
			else if ( command.startsWith( "SAVE." ) )
			{
				this->saveDocument();
			}
			else if ( command.startsWith( "SAVEAS." ) )
			{
				this->saveDocumentAs();
			}
			else if ( command.startsWith( "CLOSE.openocl::ui::Panel" ) || command.startsWith( "CLOSE.openocl::application::Document" ) )
			{
				//	CLOSE.openocl::ui::Panel is the result of a frame being closed.
				//	It is up to this code to determine whether the intent is to hide an
				//	auxiliary window or a document, or to close the document itself, or
				//	close the entire application.
				//
				//	A CLOSE.openocl::ui::Panel should only be found here if a top level application
				//	window is being closed.
				//
				//	CLOSE.openocl::ui::Document is the result of a Document intercepting a
				//	CLOSE.openocl::ui::Panel event - it creates this new event and adds as data
				//	the name of the document that is trying to be closed.
				//	The closeDocument( doc_name ) method should test to see if this is the main
				//	document window and if so close that document, else hide the target frame.

				if ( this->applicationFrame->getTopPanel().getId() == ae.getTargetId() )
				{
					if ( this->closeAllDocuments() )
					{
						if ( this->closeApplication() )
						{
							this->stop();
						}
					}
				} else {
					const String& doc_name = ae.getData();
					if ( 0 < doc_name.getLength() )
					{
						this->closeDocument( doc_name );
					} else {
						this->closeDocument();
					}
				}
			}
			else if ( command.startsWith( "CLOSE." ) )
			{
				//	This is used to handle application defined close buttons
		
				this->closeDocument();
			}
			else if ( command.startsWith( "CLOSE_FRAME_REQUEST." ) )
			{
				//	If all documents cannot be safely closed, abort the close.
				if ( !this->closeAllDocuments() )
				{
					Object& obj = const_cast<Object&>( ae.getEventSource() );
					Panel& panel = dynamic_cast<Panel&>( obj );
					panel.setContinueClose( false );
				}
			}
			else if ( command.startsWith( "QUIT." ) )
			{
				//	This is used to handle application defined quite buttons
		
				if ( this->closeAllDocuments() )
				{
					if ( this->closeApplication() )
					{
						this->stop();
					}
				}
			}
			else if ( command.startsWith( "CLOSE_FRAME." ) )
			{
				//	This is used to handle application defined quite buttons
		
				if ( this->closeAllDocuments() )
				{
					if ( this->closeApplication() )
					{
						this->stop();
					}
				}
			}
			else if ( 0 == ae.getCommand().compareTo( "openocl::ui::TabbedView" ) )
			{
				this->getApplicationFrame().getTopPanel().relayout();
			} else {
				this->deliverActionEvent( ae );
			}


			//	Redraw the frame just in case it needs it.
			//	Originally put this in because toolbar buttons
			//	that activate dialogs were not getting repainted
			//	to show they were no longer pressed.
			//
			//	Unfortunately this is causing redraw problems :(
			//
			//	this->applicationFrame->getTopPanel().redrawAll();

		} catch ( Exception* ex ) {
#ifdef PRINT_EXCEPTIONS
			fprintf( stderr, "Application::deliverEvent(): %s\n", ex->getMessage().getChars() );
#endif
			delete ex;
		}
		break;
	}
}

//-----------------------------------------------------------------------
//	public virtual methods (Application)
//-----------------------------------------------------------------------

void
Application::saveDocument()
{
	Document& doc = this->getActiveDocument();
	doc.saveDocument();
}

void
Application::saveDocumentAs()
{
	Document& doc = this->getActiveDocument();
	doc.setDocumentLocation( "" );
	doc.saveDocument();
}

bool
Application::closeDocument()
{
	bool closed = true;
	
	try
	{
		Document& doc = this->getActiveDocument();
		if ( doc.hasChanged() )
		{
			closed = false;
			
			Panel& panel = this->getApplicationFrame().getTopPanel();
			FormattedString message( "%s has not been saved!", doc.getDisplayName().getChars() ); 
			WarningBox warning_box( message, "Discard", "Cancel", "Save" );
			Dialog::showDialog( panel, warning_box, 0xFFFF, 0xFFFF );
			switch ( warning_box.getLastButtonPressed() )
			{
			case 0:
				doc.setChanged( false );  // Mark the document as unchanged so that it can be discarded
				break;
			case 2:
				this->saveDocument();
				break;
			default:
				closed = false; // Cancel
			}
		}
		
		if ( !doc.hasChanged() )
		{
			Document* doc = this->removeActiveDocument();
			doc->stop();
			doc->wait();
			delete doc;
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		closed = false;
	}
	return closed;
}

bool
Application::closeDocument( const String& documentName )
{
	bool closed = true;
	try
	{
		Document* doc = this->removeDocument( documentName );
		doc->stop();
		doc->wait();
		delete doc;
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		closed = false;
	}
	return closed;
}

bool
Application::closeAllDocuments()
{
	bool closed_all = false;

	while ( this->closeDocument() )
	{
	}

	try
	{
		this->getActiveDocument();
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		closed_all = true;
	}

	return closed_all;
}

bool
Application::closeApplication()
{
	if ( this->closeAllDocuments() )
	{
		return true;
	} else {
		return false;
	}
}

//-----------------------------------------------------------------------
//	public methods
//-----------------------------------------------------------------------

bool
Application::loadConfiguration()
{
	return false;
}

bool
Application::loadPreferences()
{
	bool loaded = false;

	const String& app_name = this->getApplicationTitle();
	//fprintf( stderr, "Application::loadPreferences: %s\n", app_name.getChars() );

	User user;
	const String& user_dir_location = user.getUserDirectory();
	StringBuffer sb;
	sb.append( user_dir_location );
	sb.append( "/Preferences/" );
	sb.append( app_name );
	
	Directory home_dir( sb.asString() );

	if ( home_dir.exists() )
	{
		sb.append( "/" );
		sb.append( app_name );
		sb.append( ".conf" );
		File config_file( sb.asString() );
		if ( config_file.exists() )
		{
			FileInputStream is( config_file );
			LineParser lp( is );
			
			while ( ! lp.hasFinished() )
			{
				String* line = lp.readLine();
				StringTokenizer st( *line );
				st.setDelimiters( " =" );
				if ( st.hasMoreTokens() )
				{
					String* key = st.nextToken();
					if ( st.hasMoreTokens() )
					{
						String* value = st.nextToken();
						this->preferences->setValue( key, value );
					} else {
						delete key;
					}
				}
				delete line;
			}
			loaded = true;
			is.close();
#ifdef DEBUG_OPENOCL_APPLICATION_APPLICATION
			fprintf( stderr, "Application::loadPreferences: loaded %s\n", sb.asString().getChars() );
#endif
		}
	}	

	return loaded;
}

void
Application::start()
{
	Panel& top = this->applicationFrame->getTopPanel();
	top.getVisual().getOffscreenImageManager().cacheImages( top.getGraphicsContext() );

	this->applicationFrame->show( Panel::ACTIVE );
	this->visual->enterEventLoop( &this->run );
}

void
Application::stop()
{
	this->run = false;
}

void
Application::addApplicationMenuItem( const String& menuId, MenuItem& aMenuItem )
{
	this->menuGroup->add( aMenuItem );
}

void
Application::removeApplicationMenuItem( MenuItem& aMenuItem )
{
	this->menuGroup->remove( aMenuItem );
}

void
Application::addToolBar( const String& toolBarId, ToolBar& aToolBar )
{
	Object* obj = &aToolBar;
	StringKey key( toolBarId );
	this->toolbars->put( key, (void*) obj );
	this->toolBarGroup->addToolBar( aToolBar );
}

void
Application::removeToolBar( const String& toolBarId )
{
	StringKey key( toolBarId );
	Object* obj = reinterpret_cast<Object*>( this->toolbars->removeValue( key ) );
	ToolBar* toolbar = dynamic_cast<ToolBar*>( obj );
	this->toolBarGroup->removeToolBar( *toolbar );
}

void
Application::addDocument( Document* aDocument )
{
	aDocument->setDocumentKey( Application::nextDocumentId++ );
	if ( this->documentManager )
	{
		this->documentManager->addDocument( aDocument );
	} else {
		fprintf( stderr, "Application::addDocument: missing document manager, aborting!\n" );
		abort();
	}
}

Document*
Application::removeDocument( const String& documentId )
throw (NoSuchElementException*)
{
	Document* doc = null;
	fprintf( stderr, "Application::removeDocument( %s )\n", documentId.getChars() );

	if ( this->documentManager )
	{
		doc = this->documentManager->removeDocument( documentId );
	} else {
		fprintf( stderr, "Application::removeDocument: missing document manager, aborting!\n" );
		abort();
	}

	return doc;
}

Document*
Application::removeActiveDocument()
throw (NoSuchElementException*)
{
	Document* document = null;

	if ( this->documentManager )
	{
		document = this->documentManager->removeActiveDocument();
	}
	else
	{
		fprintf( stderr, "Application::removeActiveDocument: missing document manager, aborting!\n" );
		abort();
	}
	return document;
}

void
Application::documentToFront( const String& documentId )
{
}

void
Application::setApplicationTitle( const String& applicationTitle )
{
	delete this->applicationTitle;
	this->applicationTitle = new String( applicationTitle );
	this->applicationFrame->setName( applicationTitle );
	this->masterMenu->setLabel( applicationTitle );
}

void
Application::setApplicationLogo( const String& anImageId )
{
	this->toolBarGroup->setLogoId( anImageId );
}

/*
void
Application::setCurrentDocumentId( const String& aDocumentId )
{
	delete this->currentDocumentId;
	this->currentDocumentId = new String( aDocumentId );
}
*/

Visual&
Application::getVisual()
{
	return *this->visual;
}

ApplicationFrame&
Application::getApplicationFrame()
{
	return *this->applicationFrame;
}

Document&
Application::getActiveDocument()
throw (NoSuchElementException*)
{
	Document* document = null;
	
	if ( this->documentManager )
	{
		document = &this->documentManager->getActiveDocument();
	}
	else
	{
		//Component& com = this->tabbedView->getActiveComponent();
		//
		//Iterator* keys = this->documents->keys();
		//while ( keys->hasNext() )
		//{
		//	const Key& key = dynamic_cast<const Key&>( keys->next() );
		//	const Object& obj = this->documents->getValue( key );
		//	const Document& doc = dynamic_cast<const Document&>( obj );
		//	const Component& view_com = doc.getViewComponent();
		//	
		//	if ( &com == &view_com )
		//	{
		//		Object& obj = this->documents->getValue( key );
		//		document = &dynamic_cast<Document&>( obj );
		//		break;
		//	}
		//}
		//delete keys;
	}
	
	if ( !document )
	{
		throw new NoSuchElementException();
	}
	
	return *document;
}

//-----------------------------------------------------------------------------------
//	public constant methods
//-----------------------------------------------------------------------------------

const String&
Application::getApplicationTitle() const
{
	return *this->applicationTitle;
}

unsigned int
Application::getNrOfDocuments() const
{
	return this->documentManager->getNrOfDocuments();
}

bool
Application::hasOpen( const openocl::base::String& aDocumentName ) const
{
	return this->documentManager->hasOpen( aDocumentName );
}

/*
const String&
Application::getCurrentDocumentId() const
{
	return *this->currentDocumentId;
}
*/

const Properties&
Application::getConfiguration() const
{
	return *this->configuration;
}

const Properties&
Application::getPreferences() const
{
	return *this->preferences;
}





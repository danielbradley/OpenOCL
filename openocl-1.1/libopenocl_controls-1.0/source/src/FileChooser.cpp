/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/FileBox.h"
#include "openocl/controls/FileChooser.h"
#include "openocl/controls/LocationBar.h"
#include "openocl/controls/ShortcutBox.h"
#include "openocl/controls/WarningBox.h"
#include <openocl/base/Environment.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/User.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/Path.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/ButtonGroup.h>
#include <openocl/ui/ColumnLayout.h>
#include <openocl/ui/ComboBox.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/LayoutManager.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/StackedLayout.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/util/StringTokenizer.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

namespace openocl {
	namespace controls {
		namespace filechoosercontrol {

class InfoContainer : public Container
{
public:
	InfoContainer()
	: Container(), filename( "filename" ), extension( "extension" )
	{
		this->add( this->filename );
		this->add( this->extension );
	}
	
	~InfoContainer()
	{
		this->remove( this->filename );
		this->remove( this->extension );
	}
	
	TextField filename;
	ComboBox extension;
};

class ButtonsContainer : public ButtonGroup
{
public:
	ButtonsContainer()
	: ButtonGroup( LayoutManager::NORTHWARD | LayoutManager::RIGHT | LayoutManager::BOTTOM ),
	  confirm( "Confirm", "CONFIRMED", "Confirm" ),
	  cancel( "Cancel", "CANCELED", "Cancel" )
	{
		this->add( this->confirm );
		this->add( this->cancel );
	}
	
	~ButtonsContainer()
	{
		this->remove( this->confirm );
		this->remove( this->cancel );
	}
	
	Button confirm;
	Button cancel;
};

class BottomContainer : public Container
{
public:
	BottomContainer()
	: Container()
	{
		//this->setLayout( new ColumnLayout() );
		this->add( this->info );
		this->add( this->buttons );
	}
	
	~BottomContainer()
	{
		this->remove( this->info );
		this->remove( this->buttons );
	}
	
	InfoContainer info;
	ButtonsContainer buttons;
};

class FileBoxContainer : public Container
{
public:
	FileBoxContainer( const String& location )
	: Container(), fileBox( location )
	{
		this->setLayout( new StackedLayout( LayoutManager::NORTHWARD ) );
		this->add( this->fileBox );
		this->add( this->bottom );
	}
	
	~FileBoxContainer()
	{
		this->remove( this->fileBox );
		this->remove( this->bottom );
	}
	
	FileBox fileBox;
	BottomContainer bottom;
};

class NavContainer : public Container
{
public:
	NavContainer( const String& location )
	: Container(), fileBoxContainer( location )
	{
		this->setLayout( new ColumnLayout() );
		this->add( this->shortcuts );
		this->add( this->fileBoxContainer );
	}
	
	~NavContainer()
	{
		this->remove( this->shortcuts );
		this->remove( this->fileBoxContainer );
	}

	ShortcutBox      shortcuts;
	FileBoxContainer fileBoxContainer;
};

class BaseContainer : public OpaqueContainer
{
public:
	BaseContainer( const String& location )
	: OpaqueContainer( Color::DIALOG ), locationBar( "LocationValue" ), navigation( location )
	{
		this->setName( "openocl::controls::filechoosercontrol::BaseContainer" );
		this->locationBar.setLocation( location );
		
		this->add( this->locationBar );
		this->add( this->navigation );
	}
	
	~BaseContainer()
	{
		this->remove( this->locationBar );
		this->remove( this->navigation );
	}

	LocationBar locationBar;
	NavContainer navigation;
};
	

};};};

FileChooser::FileChooser()
: Control( "openocl::controls::FileChooser" )
{
	Path* current = Path::getCurrentDirectory();

	this->prefix = new String( "Choose file: " );
	this->location = new String( current->getViewable() );
	this->filter = new String( "*.* (All files)" );
	this->warnIfExists = false;
	
	//	Initialize base container
	{
		openocl::controls::filechoosercontrol::BaseContainer* base =
			new openocl::controls::filechoosercontrol::BaseContainer( *this->location );
		FocusManager& fm = this->getFocusManager();
		fm.addComponent( base->locationBar );
		fm.addComponent( base->navigation.shortcuts );
		fm.addComponent( base->navigation.fileBoxContainer.fileBox );
		fm.addComponent( base->navigation.fileBoxContainer.bottom.info.filename );
		fm.addComponent( base->navigation.fileBoxContainer.bottom.info.extension );
		fm.addComponent( base->navigation.fileBoxContainer.bottom.buttons.confirm );
		fm.addComponent( base->navigation.fileBoxContainer.bottom.buttons.cancel );

		this->base = base;
		this->base->addEventListener( *this );
		this->setControlComponent( *this->base );
	}
	
	delete current;

}

FileChooser::~FileChooser()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;

	delete this->prefix;
	delete this->location;
	delete this->filter;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
FileChooser::handleActionEvent( const ActionEvent& ae )
{
	bool handled = false;

	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	const String& command  = ae.getCommand();
	const String& location = ae.getData();
		
	Path path( location );

	if ( command.startsWith( "CHANGE_DIR.openocl::controls::LocationBar" ) )
	{
		this->setLocation( location );
		base->navigation.fileBoxContainer.bottom.info.filename.setData( "" );
		base->navigation.fileBoxContainer.fileBox.setLocation( location );
		handled = true;
	}
	else if ( command.startsWith( "SELECT_FILE.openocl::controls::LocationBar" ) )
	{
		Path path( location );
		if ( Path::FILE == path.getType() )
		{
			base->navigation.fileBoxContainer.bottom.info.filename.setData( path.getBasename() );		
		}
		handled = true;
	}
	else if ( command.startsWith( "CHANGE_DIR.openocl::controls::ShortcutBox" ) )
	{
		this->setLocation( location );
		base->locationBar.setLocation( path.getViewable() );
		base->navigation.fileBoxContainer.fileBox.setLocation( location );
		handled = true;
	}
	else if ( command.startsWith( "CHANGE_DIR.openocl::controls::FileBox" ) )
	{
		this->setLocation( location );
		base->locationBar.setLocation( path.getViewable() );
		handled = true;
	}
	else if ( command.startsWith( "SELECT_FILE.openocl::controls::FileBox" ) )
	{
		Path path( location );
		base->locationBar.setLocation( path.getViewable() );
		if ( Path::FILE == path.getType() )
		{
			base->navigation.fileBoxContainer.bottom.info.filename.setData( path.getBasename() );
		}
		handled = true;
	}
	else if ( command.startsWith( "CANCELED" ) )
	{
		ActionEvent ae( *this, 0, "CANCELED.openocl::controls::FileChooser.CLICKED", base->locationBar.getLocation() );
		this->fireEvent( ae );
		handled = true;
	}
	else if ( command.startsWith( "CONFIRMED" ) )
	{
		Path path( base->locationBar.getLocation() );
		StringBuffer sb;
		if ( Path::DIRECTORY == path.getType() )
		{
			sb.append( path.getAbsolute() );
		} else {
			sb.append( path.getDirname() );
		}

		if ( '/' != sb.charAt( sb.getLength() - 1 ) )
		{
			sb.append( '/' );
		}

		const String& filename = base->navigation.fileBoxContainer.bottom.info.filename.getData();
		sb.append( filename );

		if ( false )
		{
			StringTokenizer st( base->navigation.fileBoxContainer.bottom.info.extension.getData() );
			st.setDelimiter( '.' );
			{
				String* extension = null;
				if ( st.hasMoreTokens() )
				{
					delete st.nextToken();
					st.setDelimiter( ' ' );
					if ( st.hasMoreTokens() )
					{
						extension = st.nextToken();
						if ( false == filename.endsWith( *extension ) )
						{
							sb.append( '.' );
							sb.append( *extension );
						}
					}
				}
				delete extension;
			}		
		}
		
		bool save = true;
		Path viewable( sb.asString() );

		if ( this->warnIfExists && viewable.exists() )
		{
			WarningBox warning( "Warning! File already exists", "Proceed", "Cancel" );
			if ( !Dialog::showDialog( this->getContainingPanel(), warning, 1000, 1000 ) )
			{
				save = false;
			}
		}
				
		if ( save )
		{
			delete this->location;
			this->location = new String( viewable.getViewable() );
			base->navigation.fileBoxContainer.bottom.info.filename.setData( "" );
			
			//fprintf( stdout, "FileChooser::deliverEvent(): Chosen: %s\n", viewable.getViewable().getChars() );
				
			ActionEvent ae( *this, 0, "CONFIRMED.openocl::controls::FileChooser.CLICKED", viewable.getViewable() );
			this->fireEvent( ae );
		}
		handled = true;
	} else {
		//fprintf( stderr, "FileChooser::handleActionEvent: %s %s\n", command.getChars(), location.getChars() );
	}

	return handled;
}

void
FileChooser::setContainingPanel( Panel& aPanel )
{
	this->Control::setContainingPanel( aPanel );
	this->resetTitle();
}


void
FileChooser::resetTitle()
{
	Path path( *this->location );

	StringBuffer sb;
	sb.append( *this->prefix );
	sb.append( path.getViewable() );

	try
	{
		this->getContainingPanel().setName( sb.asString() );
	} catch ( Exception* ex ) {
		delete ex;
	}
}	

void
FileChooser::setFilename( const String& filename )
{
	Path path( *this->location );
	const String& directory = path.getDirectory();
	StringBuffer sb;
	sb.append( directory );
	sb.append( path.getFileSeparator() );
	sb.append( filename );
	
	delete this->location;
	this->location = sb.toString();
	this->resetTitle();
	this->reset();
}

void
FileChooser::setLocation( const String& location )
{
	delete this->location;
	this->location = new String( location );
	this->resetTitle();
	this->reset();
}

void
FileChooser::setPrefix( const String& aPrefix )
{
	delete this->prefix;
	this->prefix = new String( aPrefix );
	this->resetTitle();
}

void
FileChooser::setWarnIfExists( bool warnIfExists )
{
	this->warnIfExists = warnIfExists;
}

void
FileChooser::setDefaultFilterExtension( const String& filterExtension )
{
	delete this->filter;
	this->filter = new String( filterExtension );

	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	base->navigation.fileBoxContainer.bottom.info.extension.setDefaultEntry( filterExtension );
}

void
FileChooser::addFilterExtension( const openocl::base::String& filterExtension )
{
	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	base->navigation.fileBoxContainer.bottom.info.extension.addEntry( filterExtension );
}

const String&
FileChooser::getChosenFile() const
{
	return *this->location;
}

const String&
FileChooser::getFilter() const
{
	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	return base->navigation.fileBoxContainer.bottom.info.extension.getData();
}

const String&
FileChooser::getLocation() const
{
	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	return base->navigation.fileBoxContainer.fileBox.getLocation();
}
//----------------------------------------------------------------------------------
//	private
//----------------------------------------------------------------------------------

void
FileChooser::reset()
{
	openocl::controls::filechoosercontrol::BaseContainer* base =
			dynamic_cast<openocl::controls::filechoosercontrol::BaseContainer*>( this->base );

	Path path( *this->location );
	const String& directory = path.getDirectory();
	
	base->locationBar.setLocation( directory );
	base->navigation.fileBoxContainer.fileBox.setLocation( directory );

	if ( Path::DIRECTORY != path.getType() )
	{
		base->navigation.fileBoxContainer.bottom.info.filename.setData( path.getBasename() );
	}
}

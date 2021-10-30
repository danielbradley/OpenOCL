/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/FileBox.h"

#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/formats/XPixmap.h>
#include <openocl/io/Directory.h>
#include <openocl/io/Path.h>
#include <openocl/ui/Blank.h>
#include <openocl/ui/BorderLayout.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/Dimensions.h>
#include <openocl/ui/DirectedLayout.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/PanelScrollPane.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Queue.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::imaging::formats;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int FileBox::padding = 4;

namespace openocl {
	namespace controls {
		namespace fileboxcontrol {

class FileContainer : public Container
{
public:
	FileContainer( FocusManager& fm )
	: Container(), icongroup( null ), scrollPane( null ), location( null ), filelist( null )
	{
		this->setName( "file_container" );
		this->setLayout( new DirectedLayout( LayoutManager::SOUTHWARD ) );
		this->showHidden = false;
		this->location = new String();

		this->icongroup = new IconGroup( IconGroup::ARRANGED );
		this->icongroup->setName( "icongroup" );
		this->icongroup->setBackground( Color::WHITE );
		this->add( *this->icongroup );

		//this->scrollPane = new ScrollPane( *this->icongroup, 0xFFFF, 0xFFFF, true );
		//this->scrollPane->setConstrainWidth( true );
		//this->add( *this->scrollPane );
	}

	~FileContainer()
	{
		//this->remove( *this->scrollPane );
		//delete this->scrollPane;
		
		if ( this->icongroup )
		{
			this->icongroup->removeAll();
			this->remove( *this->icongroup );
			delete this->icongroup;
		}
		delete this->location;
		delete this->filelist;
	}

	void deliverEvent( const Event& anEvent )
	{
		if ( Event::CHANGE_EVENT == anEvent.getType() )
		{
#ifdef DEBUG_OPENOCL_CONTROLS_FILEBOX_CHANGEEVENTS
			fprintf( stderr, "FileContainer::deliverEvent: CHANGE_EVENT\n" );
			const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
			if ( ChangeEvent::RELAYOUT == ce.getValue() )
			{
				fprintf( stderr, "\tFileContainer::deliverEvent: RELAYOUT\n" );
			}
#endif
			this->Container::deliverEvent( anEvent );
		} else {
			this->Container::deliverEvent( anEvent );
		}
	}

	void processEvent( const Event& anEvent )
	{
		if ( Event::MOUSE_EVENT == anEvent.getType() )
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			if ( this->getBounds().contains( me.getX(), me.getY() ) )
			{
				FocusEvent fe( *this, 0, FocusEvent::FOCUS_REQUESTED );
				this->fireEvent( fe );
			}
		}
		else if ( Event::ACTION_EVENT == anEvent.getType() )
		{
			// XXX
			const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
			if ( this->doRefresh && ae.getCommand().startsWith( "REFRESH." ) )
			{
				//fprintf( stderr, "FileBox::FileContainer::processEvent: %s\n", ae.getCommand().getChars() );
				this->refresh();
				this->doRefresh = false;
			}
		}
	}

/*
	bool draw( GraphicsContext& gc ) const
	{
//		fprintf( stderr, "FileBox::BaseContainer::draw()\n" );
		
		bool draw_again = false;
		//if ( this->icongroup )
		//{
		//	this->addIcons();
		//	const_cast<BaseContainer*>( this )->doLayout( gc );
		draw_again = this->Container::draw( gc );
		//	this->icongroup->removeAll();
		//}
		
		return draw_again;
	}
*/

	void setLocation( const String& location )
	{
		delete this->location;

		//fprintf( stderr, "FileBox::BaseContainer::setLocation: (1)\n" );
		if ( location.equals( "" ) )
		{
			this->location = new String( "/" );
		} else {
			Path path( location );
			const String& canonical = path.getCanonical();
			this->location = new String( canonical );
		}
		
		//fprintf( stderr, "FileBox::BaseContainer::setLocation: (2)\n" );
		if ( this->hasContainingPanel() )
		{
			//fprintf( stderr, "FileBox::BaseContainer::setLocation: (3)\n" );

			this->doRefresh = true;				//	During next getPreferredDimensions refresh will be called.
			this->getContainingPanel().requestRefresh();	//	Tell containing panel to schedule relayout.
		}
	}

	void setShowHidden( bool show )
	{
		this->showHidden = show;
	}

	void refresh()
	{
		//fprintf( stderr, "FileBox::FileContainer::refresh()\n" );
		this->repopulate();

		this->setBoundsChanged( true );			//	During next relayout this will be relayedout.
		this->icongroup->setBoundsChanged( true );	//	During next relayout icongroup will be relayedout.
		this->setRedrawAll( true );			//	During next draw this will be redrawn.
		this->icongroup->setRedrawAll( true );		//	During next draw the icongroup will be redrawn.
		this->fireChangeEvent( ChangeEvent::RELAYOUT );	//	Ensure all parents are redrawn
		this->getContainingPanel().relayout();		//	Tell containing panel to schedule relayout.
	}

	void repopulate()
	{
		this->icongroup->removeAll();
		this->processCurrentDirectory();
		this->addIcons();
	}

	void processCurrentDirectory()
	{
		Queue directories;
		Queue files;

		delete this->filelist;
		this->filelist = new Sequence();
		
		try
		{
			Directory d( *this->location );
			d.open();
			Sequence* file_list = d.retrieveFileList();
			{
				const String& absolute = d.getPath().getAbsolute();
				Iterator* it = file_list->iterator();

				while ( it->hasNext() )
				{
					const String& filename = dynamic_cast<const String&>( it->next() );
					
					bool hidden = filename.startsWith( "." ) && !filename.equals( "." ) && !filename.equals( ".." );
						
					if ( !hidden )
					{
						StringBuffer sb;
						sb.append( absolute );
						if ( '/' != sb.charAt( sb.getLength() - 1 ) )
						{
							sb.append( '/' );
						}
						sb.append( filename );

						const String& location = sb.asString();
						Path path( location );

						switch ( path.getType() )
						{
						case Path::FS_DIRECTORY:
							directories.append( sb.toString() );
							break;
						case Path::FS_FILE:
						default:
							files.append( sb.toString() );
						}
					}
				}
				delete it;
			}
			delete file_list;
				
			Object* obj = null;
			while ( (obj = directories.removeHead()) )
			{
				this->filelist->add( obj );
			}
			while ( (obj = files.removeHead()) )
			{
				this->filelist->add( obj );
			}
		} catch ( IOException* ex ) {
			delete ex;
			//this->filelist->add( new String( "." ) );
			//this->filelist->add( new String( ".." ) );
		}
	}

	void addIcons()
	{
		if ( this->filelist )
		{
			unsigned int max = this->filelist->getLength();
			for ( unsigned int i=0; i < max; i++ )
			{
				String& location = dynamic_cast<String&>( this->filelist->elementAt( i ) );

				if ( !(location.equals( "/." ) || location.equals( "/.." )) )
				{
					Icon* icon = null;
					Path path( location );

					if ( location.endsWith( "/." ) )
					{
						icon = new Icon( ".", "SELECT", "." );
					}
					else if ( location.endsWith( "/.." ) )
					{
						icon = new Icon( "..", "SELECT", ".." );
					}
					else
					{
						const String& filename = path.getBasename();
						icon = new Icon( filename, "SELECT", filename );
					}
					
					if ( icon )
					{
						icon->setMinPreferredWidth( 200 );
						icon->setMaxPreferredWidth( 200 );
						icon->setData( path.getViewable() );

						//
						//  This is causing the program to crash with wierd double
						//  dealloction of a XGC later on.
						//
						switch ( path.getType() )
						{
						case Path::FS_DIRECTORY:
							icon->setImageId( "FOLDER" );
							break;
						case Path::FS_FILE:
						default:
							icon->setImageId( "UNKNOWN" );
						}

						this->icongroup->add( icon );
					}
				}
			}
		}
	}
	
	const String& getLocation() const
	{
		return *this->location;
	}

	void doLayout( const GraphicsContext& gc )
	{
		//fprintf( stderr, "FileBox::FileContainer::doLayout()\n" );
		this->Container::doLayout( gc );
	}

/*
	Dimensions
	getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
	{
		fprintf( stderr, "FileBox::getPreferredDimensions()\n" );
		if ( this->doRefresh )
		{
			const_cast<FileContainer*>( this )->refresh();
			const_cast<FileContainer*>( this )->doRefresh = false;
		}
		return this->Container::getPreferredDimensions( gc, width, height );
	}
*/	
	IconGroup*  icongroup;
	PanelScrollPane* scrollPane;
	String*     location;
	bool        showHidden;
	bool        doRefresh;
	
	openocl::util::Sequence* filelist;
};

class FileScroller : public Container
{
public:
	FileScroller( FocusManager& fm ) : Container()
	{
		this->filecontainer = new FileContainer( fm );
		this->scrollpane = new PanelScrollPane( *this->filecontainer );
		this->scrollpane->setConstrainWidth( true );
		this->add( *this->scrollpane );
	}

	~FileScroller()
	{
		this->remove( *this->scrollpane );
		delete this->scrollpane;
		delete this->filecontainer;
	}

	void setLocation( const String& location )
	{
		this->filecontainer->setLocation( location );
	}

	void setShowHidden( bool value )
	{
		this->filecontainer->setShowHidden( value );
	}

	void refresh()
	{
		this->filecontainer->refresh();
	}

	const String& getLocation() const
	{
		return this->filecontainer->getLocation();
	}

	FileContainer* filecontainer;
	PanelScrollPane*    scrollpane;
};

};};};

//-------------------------------------------------------------------------------------
//	public class
//-------------------------------------------------------------------------------------

FileBox::FileBox( const String& location )
: Control( "openocl::controls::FileBox" )
{
	this->setResizeable( true );
	//this->base = new openocl::controls::fileboxcontrol::FileContainer( this->getFocusManager() );
	this->base = new openocl::controls::fileboxcontrol::FileScroller( this->getFocusManager() );
	this->base->addEventListener( *this );
	
	this->setControlComponent( *this->base );
	this->setLocation( location );
}

FileBox::~FileBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
FileBox::handleActionEvent( const ActionEvent& anEvent )
{
	bool handled = false;

	int type = anEvent.getType();
	switch ( type )
	{
	case Event::ACTION_EVENT:
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
#ifdef DEBUG_OPENOCL_CONTROLS_FILEBOX
		fprintf( stderr, "FileBox::handleActionEvent( %s, %s )\n", ae.getCommand().getChars(), ae.getData().getChars() );
#endif
		
		//const Path& current_path = this->current->getPath();
		//StringBuffer sb;
		//sb.append( current_path.getAbsolute() );
		//if( '/' != sb.charAt( sb.getLength() - 1 ) )
		//{
			//sb.append( '/' );
		//}
		//sb.append( ae.getCommand() );

		const String& command = ae.getCommand();
		//const String& data    = ae.getData();
		
		//fprintf( stderr, "FileBox::handleActionEvent: %s -> %s\n", command.getChars(), data.getChars() );

		if ( command.startsWith( "SELECT." ) )
		{
			bool double_clicked = command.endsWith( "DOUBLE_CLICKED" );
			const String& location = ae.getData();
			Path path( location );
		
			switch ( path.getType() )
			{
			case Path::FS_DIRECTORY:
			case Path::FS_DIRECTORY_SUPER:
				// fprintf( stderr, "FileBox: changing to: %s\n", location.getAbsolute().getChars() );

				if ( double_clicked )
				{
					this->setLocation( location );
					this->fireEvent( ActionEvent( *this, 0, "CHANGE_DIR.openocl::controls::FileBox.DOUBLE_CLICKED", path.getViewable() ) );
				} else {
					this->fireEvent( ActionEvent( *this, 0, "SELECT_DIR.openocl::controls::FileBox.CLICKED", path.getViewable() ) );
				}
				handled = true;
				break;
			case Path::FS_FILE:
				this->fireEvent( ActionEvent( *this, 0, "SELECT_FILE.openocl::controls::FileBox.CLICKED", path.getViewable() ) );
				handled = true;
			}
		}
		else if ( command.startsWith( "REFRESH." ) )
		{
			//this->getContainingPanel().relayout();
			//this->doRefresh = true;
		}
		else if ( command.startsWith( "UP." ) )
		{
			Path path( this->getLocation() );
			this->setLocation( path.getDirname() );
			this->doRefresh = true;
		}
	}

	return handled;
}

//void
//FileBox::refresh( const String& changedObjectId )
//{
//	//fprintf( stderr, "FileBox::refresh()\n" );
//	dynamic_cast<openocl::controls::fileboxcontrol::FileScroller*>( this->base )->refresh();
//}

void
FileBox::doLayout( const GraphicsContext& gc )
{
	//fprintf( stderr, "FileBox::doLayout: (1)\n" );
	if ( this->doRefresh )
	{
		//fprintf( stderr, "FileBox::doLayout: (2)\n" );
		//dynamic_cast<openocl::controls::fileboxcontrol::FileContainer*>( this->base )->refresh();
		dynamic_cast<openocl::controls::fileboxcontrol::FileScroller*>( this->base )->refresh();
		this->doRefresh = false;
	}
	this->Control::doLayout( gc );
	this->setBoundsChanged( false );
}

//------------------------------------------------------------------------------
//	public methods
//------------------------------------------------------------------------------

void
FileBox::setLocation( const openocl::base::String& location )
{
	//dynamic_cast<openocl::controls::fileboxcontrol::FileContainer*>( this->base )->setLocation( location );
	dynamic_cast<openocl::controls::fileboxcontrol::FileScroller*>( this->base )->setLocation( location );
	//this->fireEvent( ActionEvent( *this, 0, "RETITLE.openocl::controls::FileBox.CHANGED", location ) );
	this->doRefresh = true;
}

void
FileBox::setShowHidden( bool show )
{
	//dynamic_cast<openocl::controls::fileboxcontrol::FileContainer*>( this->base )->setShowHidden( show );
	dynamic_cast<openocl::controls::fileboxcontrol::FileScroller*>( this->base )->setShowHidden( show );
}	

const String&
FileBox::getLocation() const
{
	//return dynamic_cast<openocl::controls::fileboxcontrol::FileContainer*>( this->base )->getLocation();
	return dynamic_cast<openocl::controls::fileboxcontrol::FileScroller*>( this->base )->getLocation();
}

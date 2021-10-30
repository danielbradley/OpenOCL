/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/ShortcutBox.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/User.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/formats/XPixmap.h>
#include <openocl/io/Directory.h>
#include <openocl/io/File.h>
#include <openocl/io/FileInputStream.h>
#include <openocl/io/Path.h>
#include <openocl/ui/DirectedLayout.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Sequence.h>
#include <openocl/xml/Tag.h>
#include <openocl/xml/XMLStreamTokenizer.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::imaging::formats;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;
using namespace openocl::xml;

namespace openocl {
	namespace controls {
		namespace shortcutboxcontrol {

class ShortcutsContainer : public Container
{
public:
	ShortcutsContainer( const String& location )
	: Container(), path( location )
	{
		this->setLayout( new DirectedLayout( LayoutManager::SOUTHWARD ) );
		this->setPadding( 7 );
		
		this->createDefaultIcons();
		//this->createIcons();
	}

	~ShortcutsContainer()
	{
	}

	void createDefaultIcons()
	{
		User user;
		{
			const String& user_dir = user.getUserDirectory();
			String name( "Desktop" );
			StringBuffer sb;
			sb.append( user_dir );
			sb.append( "/" );
			sb.append( name );

			{
				Icon* icon = new Icon( name, "CHANGE_DIR", name );
				icon->setData( sb.asString() );
				icon->setImageId( "DESKTOP" );
				icon->setMaxPreferredWidth( 100 );
				icon->setMinPreferredWidth( 100 );
				this->add( icon );
			}
			{
				Path path( user_dir );
				const String& name = path.getBasename();
				Icon* icon = new Icon( name, "CHANGE_DIR", name );
				icon->setData( user_dir );
				icon->setImageId( "HOME" );
				icon->setMaxPreferredWidth( 100 );
				icon->setMinPreferredWidth( 100 );
				this->add( icon );
			}
			{
				Path path( "/local" );
				const String& name = path.getBasename();
				Icon* icon = new Icon( name, "CHANGE_DIR", name );
				icon->setData( path.getAbsolute() );
				icon->setImageId( "LOCAL" );
				icon->setMaxPreferredWidth( 100 );
				icon->setMinPreferredWidth( 100 );
				this->add( icon );
			}
			//{
			//	Icon* icon = new Icon( name, "CHANGE_DIR", name );
			//	icon->setData( sb.asString() );
			//	icon->setImageId( "DESKTOP" );
			//	icon->setMaxPreferredWidth( 100 );
			//	icon->setMinPreferredWidth( 100 );
			//	this->add( icon );
			//}
		}
	}

	void createIcons()
	{
		switch ( this->path.getType() )
		{
		case Path::DIRECTORY:
			{
				Directory* dir = new Directory( this->path );
				dir->open();
				Sequence* file_list = dir->retrieveFileList();

				Iterator* it = file_list->iterator();
				while ( it->hasNext() )
				{
					const String& str = dynamic_cast<const String&>( it->next() );
					StringBuffer sb;
					sb.append( this->path.getViewable() );
					sb.append( this->path.getFileSeparator() );
					sb.append( str );

					Icon* icon = new Icon( "CHANGE_DIR" );
					icon->setData( sb.asString() );
					icon->setLabel( str );
					icon->setImageId( "DESKTOP" );
					icon->setMaxPreferredWidth( 100 );
					icon->setMinPreferredWidth( 100 );

					this->add( icon );
				}
				delete it;

				delete file_list;
				delete dir;
			}
			break;
		case Path::FILE:
			{
				this->loadFile( this->path );
			}
		}
	}

	void loadFile( const Path& path )
	{
		File file( path );
		FileInputStream in( file );
		in.open();
		XMLStreamTokenizer xst( in );
		int tt = xst.nextToken();
		if ( xst.TT_START_TAG == tt )
		{
			while ( xst.TT_END_TAG != (tt = xst.nextToken()) )
			{
				if ( xst.TT_EMPTY_ELEMENT == tt )
				{
					Tag tag( xst.getSVal() );
				
					String* name   = tag.attributeValue( "name", true );
					String* target = tag.attributeValue( "target", true );
					String* icon   = tag.attributeValue( "icon", true );
				
#ifdef DEBUG_OPENOCL_CONTROLS_SHORTCUTBOX
					fprintf( stderr, "ShortcutBox::loadFile: name: %s target: %s icon: %s\n", name->getChars(), target->getChars(), icon->getChars() );
#endif				
					Icon* icn = new Icon( *name, "CHANGE_DIR", *name );
					icn->setData( *target );
					icn->setImageId( *icon );
					icn->addEventListener( *this );
					icn->setMaxPreferredWidth( 100 );
					icn->setMinPreferredWidth( 100 );

					this->add( icn );
				
					delete name;
					delete target;
					delete icon;
				}
			}
		}
	}

	Path path;
};

class BaseContainer : public Container
{
public:
	BaseContainer( const String& location )
	: Container()
	{
		this->shortcuts = new ShortcutsContainer( location );
		this->add( *this->shortcuts );
	}

	~BaseContainer()
	{
		this->remove( *this->shortcuts );
		delete this->shortcuts;
	}

	void processEvent( const Event& anEvent )
	{
		switch ( anEvent.getType() )
		{
		case Event::MOUSE_EVENT:
			{
				const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
				int me_x = me.getX();
				int me_y = me.getY();
				
				if ( this->getBounds().contains( me_x, me_y ) )
				{
					FocusEvent fe( *this, 0, FocusEvent::FOCUS_REQUESTED );
					this->fireEvent( fe );
				}
			}
		}
	}
	
	ShortcutsContainer* shortcuts;
};

};};};

ShortcutBox::ShortcutBox()
: Control( "openocl::controls::ShortcutBox" )
{
	User user;
	StringBuffer sb;
	sb.append( user.getUserDirectory() );
	sb.append( "/Preferences/OpenOCL/shortcuts.xml" );

	{
		openocl::controls::shortcutboxcontrol::BaseContainer* base =
				new openocl::controls::shortcutboxcontrol::BaseContainer( sb.asString() );
		FocusManager& fm = this->getFocusManager();
		fm.addComponent( *base->shortcuts );
		this->base = base;
		this->setControlComponent( *this->base );
		this->base->addEventListener( *this );
	}
	this->setMargin( 10 );
}

ShortcutBox::ShortcutBox( const String& location )
: Control( "openocl::controls::ShortcutBox" )
{
	{
		openocl::controls::shortcutboxcontrol::BaseContainer* base =
				new openocl::controls::shortcutboxcontrol::BaseContainer( location );
		FocusManager& fm = this->getFocusManager();
		fm.addComponent( *base->shortcuts );
		this->base = base;
		this->setControlComponent( *this->base );
		this->base->addEventListener( *this );
	}
	this->setMargin( 10 );
}

ShortcutBox::~ShortcutBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
ShortcutBox::handleActionEvent( const ActionEvent& anActionEvent )
{
	bool handled = false;
	const String& command = anActionEvent.getCommand();
	if ( command.startsWith( "CHANGE_DIR." ) )
	{
		const String& data    = anActionEvent.getData();
		this->fireEvent( ActionEvent( *this, 0, "CHANGE_DIR.openocl::controls::ShortcutBox.CLICKED", data ) );
		handled = true;
	}
	return handled;
}

bool
ShortcutBox::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	Color original_background( gc.getBackground() );

	Region bounds    = this->getBounds();
	Region box       = this->getBox();

	this->Control::draw( gc );
	this->drawInward3DBox( gc, box, Color::SHINE1, Color::SHADE2 );

/*
	gc.setBackground( Color::TOOLBAR );

		//	Background
		gc.setForeground( Color::TOOLBAR );
		gc.setFill( Color::TOOLBAR );
		gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );

		//	Draw shortcuts
		gc.setForeground( Color::SHADOW );
		gc.drawLine( shortcuts.x1, shortcuts.y1, shortcuts.x1, shortcuts.y2 );
		gc.drawLine( shortcuts.x1, shortcuts.y1, shortcuts.x2, shortcuts.y1 );
		gc.setForeground( Color::SHINY );
		gc.drawLine( shortcuts.x2, shortcuts.y2, shortcuts.x1, shortcuts.y2 );
		gc.drawLine( shortcuts.x2, shortcuts.y2, shortcuts.x2, shortcuts.y1 );

	if ( this->icons->getLength() )
	{

		gc.setClip( shortcuts.x1 + 1, shortcuts.y1 - 1, shortcuts.width - 2, shortcuts.height - 1 );
		{
			Iterator* it = this->icons->iterator();
			Region icon;
			while ( it->hasNext() )
			{
				const Component& com = dynamic_cast<const Component&>( it->next() );
				com.draw( gc );
				icon.x1 = com.getX();
				icon.y1 = com.getY();
				icon.width = com.getWidth();
				icon.height = com.getHeight();
				icon.x2 = icon.x1 + icon.width - 1;
				icon.y2 = icon.y1 + icon.height - 1;
			
				//	Button box around icon
				gc.setForeground( Color::SHADOW );
				gc.drawLine( shortcuts.x2 - 1, icon.y2, shortcuts.x2 - 1, icon.y1 );
				gc.drawLine( shortcuts.x2 - 1, icon.y2, shortcuts.x1 + 1, icon.y2 );
				gc.setForeground( Color::SHINY );
				gc.drawLine( shortcuts.x1 + 1, icon.y1, shortcuts.x2 - 1, icon.y1 );
				gc.drawLine( shortcuts.x1 + 1, icon.y1, shortcuts.x1 + 1, icon.y2 );
			}
			delete it;
		}
		gc.popClip();

		//this->desktop->draw( gc );

		//gc.setForeground( Color::SHADOW );
		//gc.drawRectangle( shortcuts.x1, shortcuts.y1, shortcuts.width, shortcuts.height );
	}

#ifdef DEBUG_OPENOCL_CONTROLS_SHORTCUTBOX
	gc.setForeground( Color::RED );
	gc.drawRectangleRegion( bounds );
	gc.setForeground( Color::GREEN );
	gc.drawRectangleRegion( shortcuts );
#endif
*/

	return draw_again;
}

/*
void
ShortCutBox::createIcons()
{
	switch ( this->source->getType() )
	{
	case Path::DIRECTORY:
		{
			Directory* dir = new Directory( *this->source );
			dir->open();
			Sequence* file_list = dir->retrieveFileList();

			Iterator* it = file_list->iterator();
			while ( it->hasNext() )
			{
				const String& str = dynamic_cast<const String&>( it->next() );
				StringBuffer sb;
				sb.append( this->source->getViewable() );
				sb.append( this->source->getFileSeparator() );
				sb.append( str );

				Icon* icon = new Icon( "OPEN" );
				icon->setData( sb.asString() );
				icon->setLabel( str );
				icon->setImageId( "DESKTOP" );

				this->icons->add( icon );
			}
			delete it;

			delete file_list;
			delete dir;
		}
		break;
	case Path::FILE:
		{
			this->loadFile( *this->source );
		}
	}
}

void
ShortcutBox::loadFile( const Path& path )
{
	File file( path );
	FileInputStream in( file );
	in.open();
	XMLStreamTokenizer xst( in );
	int tt = xst.nextToken();
	if ( xst.TT_START_TAG == tt )
	{
		while ( xst.TT_END_TAG != (tt = xst.nextToken()) )
		{
			if ( xst.TT_EMPTY_ELEMENT == tt )
			{
				Tag tag( xst.getSVal() );
				
				String* name   = tag.attributeValue( "name", true );
				String* target = tag.attributeValue( "target", true );
				String* icon   = tag.attributeValue( "icon", true );
				
#ifdef DEBUG_OPENOCL_CONTROLS_SHORTCUTBOX
				fprintf( stderr, "ShortcutBox::loadFile: name: %s target: %s icon: %s\n", name->getChars(), target->getChars(), icon->getChars() );
#endif				
				Icon* icn = new Icon( *name, "OPEN", *name );
				icn->setData( *target );
				icn->setImageId( *icon );
				icn->addEventListener( *this );

				this->icons->add( icn );
				
				delete name;
				delete target;
				delete icon;
			}
			
		}
	}
}
*/

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/FileBox.h"
#include "openocl/controls/NavigatorBox.h"
#include "openocl/controls/ShortcutBox.h"
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
#include <openocl/ui/DirectedLayout.h>
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
		namespace navigatorboxcontrol {

class NavigatorContainer : public OpaqueContainer
{
public:
	NavigatorContainer( const String& location, FocusManager& fm )
	: OpaqueContainer( Color::DIALOG ), filebox( location )
	{
		this->setName( "openocl::controls::navigatorboxcontrol::NavigatorContainer" );
		this->setLayout( new DirectedLayout( LayoutManager::EASTWARD ) );
		this->chosen = new String();

		this->add( this->shortcuts );
		this->add( this->filebox );
		
		fm.addComponent( this->shortcuts );
		fm.addComponent( this->filebox );
	}
	
	~NavigatorContainer()
	{
		this->remove( this->shortcuts );
		this->remove( this->filebox );
		delete chosen;
	}

	void deliverEvent( const Event& anEvent )
	{
		flags type = anEvent.getType();
		if ( Event::ACTION_EVENT == type )
		{
			const ActionEvent& anActionEvent = dynamic_cast<const ActionEvent&>( anEvent );
			const String& command = anActionEvent.getCommand();
			//fprintf( stderr, "NavigatorBox::NavigatorContainer::deliverEvent: %s\n", command.getChars() );
			if ( command.startsWith( "SELECTED." ) || command.startsWith( "CHANGE_DIR." ) )
			{
				const String& data = anActionEvent.getData();
				this->filebox.setLocation( data );
			}
		}
		this->Container::deliverEvent( anEvent );
	}

	void setLocation( const openocl::base::String& location )
	{
		this->filebox.setLocation( location );
	}
	
	const String& getChosenFile() const
	{
		return *this->chosen;
	}

	const String& getLocation() const
	{
		return this->filebox.getLocation();
	}

	ShortcutBox shortcuts;
	FileBox      filebox;

	String*      chosen;
};
	

};};};

//-----------------------------------------------------------------------------------------
//	public classes
//-----------------------------------------------------------------------------------------

NavigatorBox::NavigatorBox()
: Control( "openocl::controls::NavigatorBox" )
{
	Path* current = Path::getCurrentDirectory();
	const String& viewable = current->getViewable();

	this->base = new openocl::controls::navigatorboxcontrol::NavigatorContainer( viewable, this->getFocusManager() );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );
	this->setControlTitle( viewable );

	delete current;
}

NavigatorBox::~NavigatorBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
NavigatorBox::handleActionEvent( const ActionEvent& anEvent )
{
	bool handled = false;
	return handled;
}

void
NavigatorBox::setLocation( const String& location )
{
	dynamic_cast<openocl::controls::navigatorboxcontrol::NavigatorContainer*>( this->base )->setLocation( location );
}

const String&
NavigatorBox::getChosenFile() const
{
	return dynamic_cast<openocl::controls::navigatorboxcontrol::NavigatorContainer*>( this->base )->getChosenFile();
}

const String&
NavigatorBox::getLocation() const
{
	return dynamic_cast<openocl::controls::navigatorboxcontrol::NavigatorContainer*>( this->base )->getLocation();
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/WarningBox.h"

#include "openocl/base/FormattedString.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/ButtonGroup.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

namespace openocl {
	namespace controls {
		namespace warningboxcontrol {

class ButtonsButtonGroup : public openocl::ui::ButtonGroup
{
public:
	ButtonsButtonGroup( const String& button1, const String& button2 )
	: ButtonGroup( LayoutManager::EASTWARD ),
	  confirm( "Confirm", "CONFIRMED", button1 ),
	  cancel( "Cancel", "CANCELED", button2 ),
	  option( "Option", "OPTION", "" )
	{
		this->lastButton = -1;
		this->add( this->confirm );
		if ( 0 < button2.getLength() )
		{
			this->add( this->cancel );
		}
	}

	ButtonsButtonGroup( const String& button1, const String& button2, const String& button3 )
	: ButtonGroup( LayoutManager::EASTWARD ),
	  confirm( "Confirm", "CONFIRMED", button1 ),
	  cancel( "Cancel", "CANCELED", button2 ),
	  option( "Option", "OPTION", button3 )
	{
		this->lastButton = -1;
		this->add( this->confirm );
		if ( 0 < button2.getLength() )
		{
			this->add( this->cancel );
		}
		if ( 0 < button3.getLength() )
		{
			this->add( this->option );
		}
	}

	~ButtonsButtonGroup()
	{
		this->remove( this->confirm );
		if ( 0 < this->cancel.getLabel().getLength() )
		{
			this->remove( this->cancel );
		}
		if ( 0 < this->cancel.getLabel().getLength() )
		{
			this->remove( this->option );
		}
	}

	void deliverEvent( const Event& anEvent )
	{
		
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& data    = ae.getData();

		const Sequence& components = this->getComponents();
		unsigned int max = components.getLength();

		unsigned int i;
		for ( i=0; i < max; i++ )
		{
			const Object& obj = components.elementAt( i );
			const Button& button = dynamic_cast<const Button&>( obj );
			if ( 0 == button.getLabel().compareTo( data ) )
			{
				//fprintf( stderr, "WarningBox::BBG::deliverEvent: %s == %s\n", button.getLabel().getChars(), data.getChars() );
				break;
			}
		}
		
		if ( i < max )
		{
			this->lastButton = i;
		}

		//fprintf( stderr, "WarningBox::BBG::deliverEvent: %s -> %s (%u)\n", command.getChars(), data.getChars(), this->lastButton );
		fireEvent( anEvent );
	}
	
	openocl::ui::Button confirm;
	openocl::ui::Button cancel;
	openocl::ui::Button option;

	int lastButton;
};

class Base : public openocl::ui::OpaqueContainer
{
public:
	Base( const String& message, const String& button1, const String& button2, const String& button3 )
	: OpaqueContainer( Color::DIALOG ), message( message ), buttons( button1, button2, button3 )
	{
		this->add( this->message );
		this->add( this->buttons );
	}
	
	~Base()
	{
		this->remove( message );
		this->remove( this->buttons );
	}
	
	Label message;
	ButtonsButtonGroup buttons;	
};

};};};

WarningBox::WarningBox( const String& message, const String& button1, const String& button2 )
: Control( "openocl::controls::WarningBox" )
{
	openocl::controls::warningboxcontrol::Base* base =
		new openocl::controls::warningboxcontrol::Base( message, button1, button2, "" );
	this->base = base;

	this->setControlTitle( "Warning!" );
	this->setResizeable( false );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );

	FocusManager& fm = this->getFocusManager();
	fm.addComponent( base->buttons.confirm );
	fm.addComponent( base->buttons.cancel );
	fm.setFocus( base->buttons.confirm );
}

WarningBox::WarningBox( const String& message, const String& button1, const String& button2, const String& button3 )
: Control( "openocl::controls::WarningBox" )
{
	openocl::controls::warningboxcontrol::Base* base =
		new openocl::controls::warningboxcontrol::Base( message, button1, button2, button3 );
	this->base = base;

	this->setControlTitle( "Warning!" );
	this->setResizeable( false );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );

	FocusManager& fm = this->getFocusManager();
	fm.addComponent( base->buttons.confirm );
	fm.addComponent( base->buttons.cancel );
	fm.setFocus( base->buttons.confirm );
}

WarningBox::~WarningBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
WarningBox::handleActionEvent( const ActionEvent& anActionEvent )
{
	//const String& command = anActionEvent.getCommand();
	//const String& data    = anActionEvent.getData();

	//fprintf( stderr, "WarningBox::deliverEvent: command=\"%s\" data=\"%s\"\n", command.getChars(), data.getChars() );

	return false;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

int
WarningBox::getLastButtonPressed() const
{
	const openocl::controls::warningboxcontrol::Base* base = dynamic_cast<const openocl::controls::warningboxcontrol::Base*>( this->base );
	return base->buttons.lastButton;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/DialogBox.h"

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
#include <openocl/ui/Control.h>
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
		namespace dialogboxcontrol {

class ButtonsContainer : public ButtonGroup
{
public:
	ButtonsContainer()
	: ButtonGroup( LayoutManager::EASTWARD | LayoutManager::RIGHT | LayoutManager::BOTTOM ),
	  confirm( "Confirm", "CONFIRMED", "Confirm" ),
	  cancel( "Cancel", "CANCELED", "Cancel" )
	{
		//this->setLayout( new DirectedLayout( LayoutManager::EASTWARD ) );
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

class DialogContainer : public OpaqueContainer
{
public:
	DialogContainer( Control& ctrl, FocusManager& fm )
	: OpaqueContainer( Color::DIALOG ), ctrlRef( ctrl )
	{
		this->setLayout( new DirectedLayout( LayoutManager::NORTHWARD ) );
		this->setName( "openocl::controls::dialogboxcontrol::DialogContainer" );
		this->add( this->ctrlRef );
		this->add( this->buttons );
		
		fm.addComponent( this->ctrlRef );
		fm.addComponent( this->buttons.confirm );
		fm.addComponent( this->buttons.cancel );
	}
	
	~DialogContainer()
	{
		this->remove( this->ctrlRef );
		this->remove( this->buttons );
	}

	virtual void doLayout( const GraphicsContext& gc )
	{
		this->OpaqueContainer::doLayout( gc );
	}

	Control&         ctrlRef;
	ButtonsContainer buttons;
};
	

};};};

//---------------------------------------------------------------------------------
//	public class DialogBox
//---------------------------------------------------------------------------------

DialogBox::DialogBox( Control& ctrl )
: Control( "openocl::controls::DialogBox" )
{
	this->base = new dialogboxcontrol::DialogContainer( ctrl, this->getFocusManager() );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );
	this->setControlTitle( ctrl.getControlTitle() );
}

DialogBox::~DialogBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
DialogBox::handleActionEvent( const ActionEvent& anEvent )
{
	bool result = false;
	return result;
}

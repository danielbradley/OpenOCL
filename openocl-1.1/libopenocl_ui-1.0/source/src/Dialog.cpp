/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Dialog.h"

#include "openocl/ui/Button.h"
#include "openocl/ui/ButtonGroup.h"
#include "openocl/ui/Control.h"
#include "openocl/ui/TextField.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/FormattedString.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

bool
Dialog::showDialog( Panel& guardian, Component& com, unsigned int maxWidth, unsigned int maxHeight )
{
	bool confirmed = false;

	flags d_type = Panel::DIALOG;

	String* control_name = null;
	try
	{
		Control* ctrl = dynamic_cast<Control*>( &com );
		if ( ctrl )
		{
			control_name = new String( ctrl->getControlTitle() );
			
			if ( !ctrl->isResizeable() )
			{
				d_type = Panel::FIXED_DIALOG;
			}
		}
	} catch ( Exception* ex ) {
		fprintf( stderr, "Dialog::showDialog: unexpected exception, aborting!\n" );
		delete ex;
	}

	Dialog* dialog = new Dialog( guardian, d_type );
	Panel& panel = dialog->getTopPanel();
	
	int gx = guardian.getAbsoluteX();
	int gy = guardian.getAbsoluteY();

	//fprintf( stderr, "Dialog::showDialog() at %i:%i\n", gx, gy );

	try
	{
		//	Mac OS X X11
		//	If dialog->positionAndResize is before dialog->setVisible the
		//	new position is ignored. Need to try this on other platforms
		//	to see if the same holds true.
	
		if ( null == control_name )
		{
			panel.setName( com.getName() );
		} else {
			panel.setName( *control_name );
		}
		panel.setBackground( Color::RED );
		panel.add( com );
		panel.forwardRedrawAllChangeEvent();
		dialog->positionAndResize( gx, gy, maxWidth, maxHeight );
		dialog->toFront();
		dialog->setVisible( true );
		//dialog->positionAndResize( gx, gy, 800, 600 );
		//dialog->resize( 800, 600 );
		
		dialog->enterEventLoop( dialog->getLoopPointer() );
		dialog->setVisible( false );
		panel.remove( com );
		confirmed = dialog->wasConfirmed();
	} catch ( Exception* ex ) {
		delete ex;
	} catch ( ... ) {
		fprintf( stderr, "Aborting in Dialog::showDialog(): unexpected real exception\n" );
		dialog->setVisible( false );
		panel.remove( com );
	}
	delete dialog;
	delete control_name;

	//	This redraws the parent frame to get it back to its proper state.
	guardian.redrawAll();
	guardian.relayout();

	return confirmed;
}

Dialog::Dialog( Panel& guardian, mask dialogType )
: Frame( guardian.getScreen().createPanel( dialogType ) ), guardian( guardian )
{
	//this->requestGeometry( 300, 300, 100, 300, 0 );
	this->value = new String( "null" );
	this->confirmed = false;
	this->loop = true;
	this->getTopPanel().setName( "Dialog" );
}

Dialog::~Dialog()
{
	delete this->value;
}

void
Dialog::deliverEvent( const Event& anEvent )
{
	int type = anEvent.getType();
	switch ( type )
	{
	case Event::ACTION_EVENT:
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& command = ae.getCommand();
		if ( command.startsWith( "CONFIRMED." ) || command.startsWith( "OPTION" ) )
		{
			delete this->value;
			this->value = new String( ae.getData() );
			//fprintf( stderr, "Dialog::deliverEvent()\n" );
			this->confirmed = true;
			this->loop = false;
		}
		else if ( command.startsWith( "CANCELED." ) )
		{
			this->loop = false;
		}
		else if ( command.startsWith( "CLOSE_FRAME." ) )
		{
			this->loop = false;
		}
		else if ( command.startsWith( "RETITLE." ) )
		{
			this->getTopPanel().setName( ae.getData() );
		}
		break;
	}
}

void
Dialog::setVisible( bool aValue )
{
	if ( aValue )
	{
		//unsigned int width = this->getPreferredWidth();
		//unsigned int height = this->getPreferredHeight();
		//this->requestSize( width, height );
		this->guardian.show( Panel::INACTIVE );
		this->Frame::show( Panel::ACTIVE ); //setVisible( aValue );
	} else {
		this->Frame::show( Panel::HIDDEN );
		this->guardian.show( Panel::ACTIVE );
		this->guardian.raise();
	}
}

void
Dialog::enterEventLoop( bool* run )
{
	const Panel& top_panel = this->getTopPanel();
	const Visual& visual = top_panel.getVisual();
	visual.enterEventLoop( run, top_panel.getId() );
}

bool*
Dialog::getLoopPointer()
{
	return &this->loop;
}

const String&
Dialog::getValue() const
{
	return *this->value;
}

bool
Dialog::wasConfirmed() const
{
	return this->confirmed;
}

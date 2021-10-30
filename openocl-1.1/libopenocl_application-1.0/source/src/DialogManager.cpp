/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/application/DialogManager.h"
#include <openocl/base/Exception.h>
#include <openocl/base/String.h>
#include <openocl/controls/FileChooser.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/ButtonGroup.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/TextField.h>

using namespace openocl::application;
using namespace openocl::base;
using namespace openocl::controls;
using namespace openocl::ui;

void
DialogManager::showDialog( Panel& aPanel, Component& com )
{
	Dialog* dialog = new Dialog( aPanel );
	Panel& panel = dialog->getTopPanel();
	try
	{
		panel.setName( "Dialog" );
		panel.add( com );
		dialog->requestGeometry( 300, 300, 600, 600, 0 );
		dialog->setVisible( true );
		dialog->enterEventLoop( dialog->getLoopPointer() );
		dialog->setVisible( false );
		panel.remove( com );
	} catch ( Exception* ex ) {
		delete ex;
	}
	delete dialog;
}

String*
DialogManager::openDialog( Panel& aPanel )
{
	String* filename = null;

	FileChooser* file_chooser = new FileChooser();

/*
	Button* one   = new Button( "dismiss" );
	ButtonGroup* button_group = new ButtonGroup( LayoutManager::EASTWARD | LayoutManager::LEFT | LayoutManager::MIDDLE );
	button_group->add( one );
*/
	Dialog* dialog = new Dialog( aPanel );
	dialog->setName( "Open" );
	dialog->getTopPanel().add( *file_chooser );
	dialog->requestGeometry( 300, 300, 600, 600, 0 );
	dialog->setVisible( true );
	dialog->enterEventLoop( dialog->getLoopPointer() );
	filename = new String( dialog->getValue() );
	dialog->getTopPanel().remove( *file_chooser );
	dialog->setVisible( false );
	delete dialog;

	delete file_chooser;

	return filename;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ApplicationFrame.h"

#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;

//ApplicationFrame::ApplicationFrame( Visual& aVisual )
//: Frame( aVisual.getDefaultScreen().createPanel( Panel::APPLICATION ) ), visual( aVisual )
//{
//	this->getTopPanel().setName( "ApplicationFrame" );
//}

ApplicationFrame::ApplicationFrame( Visual& aVisual, bool* loop )
: Frame( aVisual.getDefaultScreen().createPanel( Panel::APPLICATION ) ), visual( aVisual ), loopPtr( loop )
{
	this->getTopPanel().setName( "ApplicationFrame" );
}

ApplicationFrame::~ApplicationFrame()
{}

//-------------------------------------------------------------------------
//	public virtual method (EventListener)
//-------------------------------------------------------------------------

void
ApplicationFrame::deliverEvent( const Event& anEvent )
{
	if ( Event::ACTION_EVENT == anEvent.getType() )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& command = ae.getCommand();
		if ( command.startsWith( "CLOSE_FRAME." ) )
		{
			*this->loopPtr = false;
		}
	}
}

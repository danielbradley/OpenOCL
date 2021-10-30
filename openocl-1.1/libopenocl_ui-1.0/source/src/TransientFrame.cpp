/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/TransientFrame.h"

#include "openocl/ui/ColumnLayout.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Screen.h"
#include "openocl/ui/Visual.h"

#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>

using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::ui;

TransientFrame::TransientFrame( Visual& aVisual ) : FloatingFrame( aVisual )
{
	this->getTopPanel().setLayout( new ColumnLayout() );
	this->requestGeometry( 0, 0, 1280, 50, 0 );
}

void
TransientFrame::deliverEvent( const Event& anEvent )
{
	IO::err().printf( "TransientFrame::deliverEvent()\n" );
	int type = anEvent.getType();
	
	switch ( type )
	{
	case Event::FOCUS_EVENT:
		const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
		switch ( fe.getFocusEventType() )
		{
		case FocusEvent::TAKE_FOCUS:
			IO::err().printf( "TransientFrame::deliverEvent(): setting visible\n" );
			this->setVisible( true );
			break;
		case FocusEvent::LOSE_FOCUS:
			IO::err().printf( "TransientFrame::deliverEvent(): setting invisible\n" );
			this->setVisible( false );
			break;
		}
	}
}

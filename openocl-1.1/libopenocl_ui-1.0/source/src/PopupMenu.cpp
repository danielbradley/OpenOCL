/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/PopupMenu.h"
#include "openocl/ui/Visual.h"

#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>

#include <cstdio>

using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

//PopupMenu::PopupMenu( Screen& screen  ) : PopupFrame( screen )
//{
//	this->items = new OpaqueContainer( Color::TOOLBAR );
//	this->getTopPanel().add( *this->items );
//}

//PopupMenu::PopupMenu( Panel& parent ) : PopupFrame( parent )
PopupMenu::PopupMenu( Panel& parent ) : PopupFrame( parent.getScreen() )
{
	this->items = new OpaqueContainer( Color::TOOLBAR );
	this->items->setEventMask( Event::MOUSE_EVENT );
	this->getTopPanel().add( *this->items );
}

PopupMenu::~PopupMenu()
{
	//	Need to remove the items from top panel before
	//	they are deleted, else when the container is destroyed
	//	attempting to dynamic cast a non-existant object will
	//	cause a seg fault.
	
	this->getTopPanel().remove( *this->items );
	delete this->items;
	//fprintf( stderr, "PoupMenu::~PopupMenu(): deleted stuff\n" );
}

void
PopupMenu::add( Component* component )
{
	this->items->add( component );
}

void
PopupMenu::add( Component& component )
{
	this->items->add( component );
}

void
PopupMenu::remove( Component& component )
{
	this->items->remove( component );
}

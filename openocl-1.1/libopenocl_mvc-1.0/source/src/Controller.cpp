/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/mvc.h"
#include "openocl/mvc/Controller.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Object.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/io/IO.h>
#include <openocl/base/Debug.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::mvc;
using namespace openocl::util;

Controller::Controller()
{
	this->eventListeners = new Sequence();
}

Controller::~Controller()
{
	delete this->eventListeners;
}

void
Controller::addEventListener( EventListener& anEventListener )
{
	this->eventListeners->add( (Object&) anEventListener );
}

void
Controller::removeEventListener( EventListener& anEventListener )
{
	unsigned int max = this->eventListeners->getLength();

	for ( unsigned int i=0; i < max; i++ )
	{
		const Object& obj = this->eventListeners->elementAt( i );
		if ( ((void*)&obj) == ((void*)&anEventListener) )
		{
			this->eventListeners->remove( i );
			this->removeEventListener( anEventListener );
			break;
		}
	}
}

void
Controller::fireEvent( const Event& anEvent )
{
	Iterator* it = this->eventListeners->iterator();
	while ( it->hasNext() )
	{
		EventListener& el = (EventListener&) it->next();
		el.deliverEvent( anEvent );
	}
	delete it;
}

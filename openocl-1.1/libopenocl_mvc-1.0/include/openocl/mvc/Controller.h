/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MVC_CONTROLLER_H
#define OPENOCL_MVC_CONTROLLER_H

#include "openocl/mvc.h"
#include <openocl/base/Object.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/event.h>
#include <openocl/util.h>

namespace openocl {
	namespace mvc {

class Controller : public openocl::base::Object,
                   public openocl::base::event::EventHandler
{
public:
	virtual ~Controller();

	// EventHandler method
	void addEventListener( openocl::base::event::EventListener& anEventListener );
	void removeEventListener( openocl::base::event::EventListener& anEventListener );

protected:
	Controller();

	// EventHandler method
	void fireEvent( const openocl::base::event::Event& event );

	openocl::util::Sequence* eventListeners;
};

};};

#endif

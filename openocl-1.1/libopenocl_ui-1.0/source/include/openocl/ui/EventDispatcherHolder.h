/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_EVENTDISPATCHERHOLDER_H
#define OPENOCL_UI_EVENTDISPATCHERHOLDER_H

#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/base/event.h>
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class EventDispatcherHolder : public openocl::base::Object
{
public:
	EventDispatcherHolder( openocl::base::event::EventDispatcher& aDispatcher );
	~EventDispatcherHolder();

	openocl::base::event::EventDispatcher& dispatcher;
};

};};

#endif

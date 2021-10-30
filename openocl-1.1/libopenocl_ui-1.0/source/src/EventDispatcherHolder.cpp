/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/EventDispatcherHolder.h"

#include <openocl/base/event/EventDispatcher.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;

EventDispatcherHolder::EventDispatcherHolder( EventDispatcher& aDispatcher )
: Object(), dispatcher( aDispatcher )
{}

EventDispatcherHolder::~EventDispatcherHolder()
{}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CARBON_CARBONEVENTMANAGER_H
#define OPENOCL_UI_CARBON_CARBONEVENTMANAGER_H

#include <openocl/base/event/EventSource.h>
#include <openocl/base/event.h>
#include <openocl/mvc/Controller.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {
		namespace carbon {

class CarbonEventManager :
public openocl::base::event::EventSource,
public openocl::mvc::Controller
{
public:
	static CarbonEventManager* eventManager;

	CarbonEventManager();
	virtual ~CarbonEventManager();

	void registerDispatcher( unsigned long int id, openocl::base::event::EventDispatcher& anEventDispatcher );
	void postEvent( openocl::base::event::Event* anEvent );

	void enterEventLoop( bool* loop, unsigned long winId );
	
private:
	openocl::util::Sequence* dispatchers;
};

};};};

#endif

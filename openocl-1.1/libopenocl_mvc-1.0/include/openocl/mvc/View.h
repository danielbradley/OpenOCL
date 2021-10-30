/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MVC_VIEW_H
#define OPENOCL_MVC_VIEW_H

#include <openocl/base/Object.h>
#include <openocl/base/event/ChangeEventListener.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/event/RefreshEventListener.h>

#include <openocl/base/event.h>
#include <openocl/mvc.h>
#include <openocl/threads.h>
#include <openocl/ui.h>
#include <openocl/util.h>

namespace openocl {
	namespace mvc {

/**
 *  The View class extends EventListener, because a View adds itself as a listener
 *  to receive ChangeEvents from its Model. <p>
 *
 *  The View class is an EventHandler, because classes can register to receive events
 *  from the view such ActionEvents (when a button is clicked).
 */

// Abstract
class View : public openocl::base::Object,
             public openocl::base::event::EventHandler,
             public openocl::base::event::EventListener,
             public openocl::base::event::ChangeEventListener,
             public openocl::base::event::RefreshEventListener
{
public:
	virtual ~View();

	//	EventListener method
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	ChangeEventListener method
	virtual void deliverChangeEvent( openocl::base::event::RefreshEvent* anEvent );

	//	RefreshEventListener method
	virtual void deliverRefreshEvent( const openocl::base::event::Event& anEvent );

	//	EventHandler method
	virtual void addEventListener( openocl::base::event::EventListener& anEventListener );
	virtual void removeEventListener( openocl::base::event::EventListener& anEventListener );

	//	public virtual methods
	virtual void requestRefresh();
	virtual void refresh( const openocl::base::String& changedObjectId ) = 0;
	
	//	public methods
	void addViewComponent( openocl::ui::Component& viewComponent );
	void removeViewComponent( openocl::ui::Component& viewComponent );
	openocl::ui::Component& getComponent();

protected:
	//  EventHandler method
	virtual void fireEvent( const openocl::base::event::Event& anEvent );

	View( Model& aModel );

private:
	void refresh();

	Model& model;

	openocl::ui::Container* documentPane;
	openocl::util::Sequence* eventListeners;
	openocl::threads::EventQueue* changeEvents;
};

};};

#endif

/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MVC_SUBVIEW_H
#define OPENOCL_MVC_SUBVIEW_H

#include "openocl/mvc.h"
#include <openocl/base/Object.h>
#include <openocl/base/event/ChangeEventListener.h>

#include <openocl/base/event.h>
#include <openocl/mvc.h>
#include <openocl/threads.h>
#include <openocl/ui.h>
#include <openocl/ui/Control.h>
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
class Subview : public openocl::ui::Control,
                public openocl::base::event::ChangeEventListener
{
public:
	virtual ~Subview();

	//	ChangeEventListener method
	virtual void deliverChangeEvent( openocl::base::event::RefreshEvent* anEvent );
	
	//	public virtual methods
	virtual void setSubviewComponent( openocl::ui::Component& viewComponent );
	
	virtual openocl::ui::Component& getSubviewComponent() throw (openocl::base::NoSuchElementException*);

	//	public methods
	virtual void refresh( const openocl::base::String& changedObjectId ) = 0;

protected:

	Subview( Model& aModel );

private:
	Model& model;
	
	openocl::ui::Component* subviewComponent;
	openocl::threads::EventQueue* changeEvents;
};

};};

#endif

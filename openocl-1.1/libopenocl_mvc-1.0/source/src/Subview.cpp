/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/mvc.h"
#include "openocl/mvc/Model.h"
#include "openocl/mvc/Subview.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/RefreshEvent.h>
#include <openocl/base/Debug.h>
#include <openocl/mvc/DocumentPaneMonitor.h>
#include <openocl/threads/EventQueue.h>
#include <openocl/threads/Mutex.h>
#include <openocl/ui/Blank.h>
#include <openocl/ui/Component.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/DocumentPane.h>
#include <openocl/ui/Panel.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::mvc;
using namespace openocl::threads;
using namespace openocl::ui;
using namespace openocl::util;

Subview::Subview( Model& aModel )
: Control( "openocl::mvc::Subview" ), model( aModel )
{
	this->model.addChangeEventListener( *this );
	this->changeEvents = new EventQueue();
}

Subview::~Subview()
{
	this->model.removeChangeEventListener( *this );
	delete this->changeEvents;
}

//------------------------------------------------------------------------------
//	public virtual methods (EventListener)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	public virtual methods (ChangeEventListener)
//------------------------------------------------------------------------------

/**
 *  The view is a listener of the Model, therefore it will be
 *  receiving Change Events through this method.
 *
 *  Subclasses that want to receive Change Events should override
 *  this method.
 */
void
Subview::deliverChangeEvent( RefreshEvent* aRefreshEvent )
{
	//this->changeEvents->postEvent( aChangeEvent );
	this->refresh( aRefreshEvent->getObjectId() );
	delete aRefreshEvent;
}

/**
 *  Subview is a component (EventHandler) of the model, which will
 *  be propagating Input Events to the view, using this method.
 *
 *  Subclasses that want to receive Input Events should override
 *  this method.
 *
 *  REMOVED
 *
 *  Actually a views components will receive their own events
 *  through the dispatch mechanism.
 *
 * void
 * Subview::dispatchEvent( Event* anEvent )
 * {
 *   Debug::entering( "Subview", "dispatchEvent", "anEvent" );
 *   delete anEvent;
 *   Debug::leaving();
 * }
 */

//------------------------------------------------------------------------------
//	public virtual methods (EventHandler)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	public virtual methods (Subview)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	public methods (Subview)
//------------------------------------------------------------------------------

void
Subview::setSubviewComponent( Component& viewComponent )
{
	this->subviewComponent = &viewComponent;
	this->setControlComponent( viewComponent );
}

Component&
Subview::getSubviewComponent()
throw (NoSuchElementException*)
{
	return *this->subviewComponent;
}

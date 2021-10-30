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
#include "openocl/mvc/Model.h"
#include "openocl/mvc/View.h"

#include <openocl/base/Iterator.h>
#include <openocl/base/Object.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/ChangeEventListener.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event/InputEvent.h>
#include <openocl/base/event/RefreshEvent.h>
#include <openocl/base/Debug.h>
#include <openocl/threads/EventQueue.h>
#include <openocl/threads/Mutex.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::mvc;
using namespace openocl::threads;
using namespace openocl::util;

Model::Model()
{
	this->progressMutex  = new Mutex( "PROGRESS" );
	this->readMutex      = new Mutex( "READ" );
	this->writeMutex     = new Mutex( "WRITE" );
	this->eventQueue     = new EventQueue();
	this->changeEventListeners = new Sequence();
	
	this->progress = 0.0;
}

Model::~Model()
{
	delete this->eventQueue;
	delete this->progressMutex;
	delete this->readMutex;
	delete this->writeMutex;
	delete this->changeEventListeners;
}

/*
void
Model::addEventListener( EventListener& anEventListener )
{
	Object* obj = dynamic_cast<Object*>( &anEventListener );
	if ( null != obj ) {
		this->eventListeners->add( obj );
	}
}
*/

/*
void
Model::removeEventListener( EventListener& anEventListener )
{
	unsigned int max = this->eventListeners->getLength();
	Object& object = dynamic_cast<Object&>( anEventListener );
	Object* obj = &object;
	void* el = (void*) obj;

	for ( unsigned int i=0; i < max; i++ )
	{
		void* obj = (void*) &this->eventListeners->elementAt( i );
		if ( obj == el )
		{
			this->eventListeners->remove( i );
			//this->removeEventListener( anEventListener );
			break;
		}
	}
}
*/
/*
void
Model::fireEvent( const Event& anEvent )
{
	Iterator* it = this->eventListeners->iterator();
	while ( it->hasNext() )
	{
		Object& obj = (Object&) it->next();
		EventListener* eventListener = dynamic_cast<EventListener*>( &obj );
		eventListener->deliverEvent( anEvent );
	}

	delete it;
}
*/

void
Model::addChangeEventListener( ChangeEventListener& aChangeEventListener )
{
	try
	{
		Object& obj = dynamic_cast<Object&>( aChangeEventListener );
		this->changeEventListeners->add( obj );
	} catch ( ... ) {
		;
	}
}

void
Model::removeChangeEventListener( ChangeEventListener& aChangeEventListener )
{
	unsigned int max = this->changeEventListeners->getLength();

	try
	{
		Object& object = dynamic_cast<Object&>( aChangeEventListener );
		Object* obj = &object;
		void* el = (void*) obj;

		for ( unsigned int i=0; i < max; i++ )
		{
			void* obj2 = (void*) &this->changeEventListeners->elementAt( i );
			if ( obj2 == el )
			{
				this->changeEventListeners->remove( i );
				break;
			}
		}
	} catch ( ... ) {
		;
	}
}

//void
//Model::resetChanged( long mask )
//{
//	this->changed ^= mask;
//}

EventQueue&
Model::getEventQueue()
{
	return *this->eventQueue;
}

//---------------------------------------------------------------
//	public pseudo-constant methods (Model)
//---------------------------------------------------------------

void
Model::lockForRead( const char* callerId ) const
{
	this->readMutex->lock( callerId );
}

bool
Model::tryLockForRead( const char* callerId ) const
{
	return this->readMutex->tryLock( callerId );
}

void
Model::unlockFromRead( const char* callerId ) const
{
	this->readMutex->unlock( callerId );
}

//---------------------------------------------------------------
//	public constant methods (Model)
//---------------------------------------------------------------

double
Model::getProgress() const
{
	double pg;
	this->progressMutex->lock( "Model::getProgress()" );
	pg = this->progress;
	this->progressMutex->unlock( "Model::getProgress()" );
	return pg;
}

const char*
Model::getStatus() const
{
	return this->status;
}

//long
//Model::hasChanged() const
//{
//	return this->changed;
//}

//---------------------------------------------------------------
//	protected methods (Model)
//---------------------------------------------------------------

void
Model::fireRefreshEvent( const char* id )
{
	String id_str( id );
	RefreshEvent refresh_event( *this, 0, id_str );
	this->fireChangeEvent( refresh_event );
}

void
Model::setChanged( const char* id )
{
	RefreshEvent refresh_event( *this, 0, id );
	this->fireChangeEvent( refresh_event );
}

void
Model::fireChangeEvent( const RefreshEvent& aRefreshEvent )
{
	unsigned int max = this->changeEventListeners->getLength();
	for ( unsigned int i=0; i < max; i++ )
	{
		Object& obj = this->changeEventListeners->elementAt( i );
		ChangeEventListener& cel = dynamic_cast<ChangeEventListener&>( obj );
		cel.deliverChangeEvent( new RefreshEvent( aRefreshEvent ) );
	}
}

void
Model::setProgress( double pg )
{
#ifndef DEBUG_OPENOCL_MVC_DISABLE_PROGRESS
	this->progressMutex->lock( "Model::setProgress()" );
	this->progress = pg;
	this->progressMutex->unlock( "Model::setProgress()" );
	this->fireRefreshEvent( "Progress" );
#endif
}

void
Model::setStatus( const char* status )
{
	this->status = status;
}

//---------------------------------------------------------------
//	protected pseudo-constant methods (Model)
//---------------------------------------------------------------

void
Model::lock( const char* callerId ) const
{
	this->readMutex->lock( callerId );
	this->writeMutex->lock( callerId );
}

void
Model::unlock( const char* callerId ) const
{
	this->writeMutex->unlock( callerId );
	this->readMutex->unlock( callerId );
}


/*
void
Model::addView( View& aView )
{
	Object* obj = dynamic_cast<Object*>( &aView );
	this->views->add( obj );
}

void
Model::removeView( View& aView )
{
	unsigned int max = this->views->getLength();
	Object& object = dynamic_cast<Object&>( aView );
	Object* obj = &object;
	void* el = (void*) obj;

	for ( unsigned int i=0; i < max; i++ )
	{
		void* obj = (void*) &this->views->elementAt( i );
		if ( obj == el )
		{
			this->views->remove( i );
			break;
		}
	}
}
*/

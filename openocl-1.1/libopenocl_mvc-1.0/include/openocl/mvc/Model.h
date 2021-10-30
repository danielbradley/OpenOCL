/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MVC_MODEL_H
#define OPENOCL_MVC_MODEL_H

#include "openocl/mvc.h"
#include <openocl/threads.h>
#include <openocl/util.h>
#include <openocl/base/Object.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event/EventHandler.h>

namespace openocl {
	namespace mvc {

// Abstract
class Model : public openocl::base::Object
//, public openocl::base::event::EventHandler //, public openocl::base::event::EventListener
{
public:
	virtual ~Model();

	// EventHandler methods
	//virtual void addEventListener( openocl::base::event::EventListener& anEventListener );
	//virtual void removeEventListener( openocl::base::event::EventListener& anEventListener );
	//virtual void fireEvent( const openocl::base::event::Event& anEvent ); 

	// EventListener methods
	//virtual void deliverEvent( const openocl::base::event::Event& anEvent ) = 0;

	//	public methods
	//virtual void addView( View& aView );
	//virtual void removeView( View& aView );

	void addChangeEventListener( openocl::base::event::ChangeEventListener& aChangeEventListener );
	void removeChangeEventListener( openocl::base::event::ChangeEventListener& aChangeEventListener );

	//void  resetChanged( long mask );

	openocl::threads::EventQueue& getEventQueue();
	
	void lockForRead( const char* callerId ) const;
	bool tryLockForRead( const char* callerId ) const;
	void unlockFromRead( const char* callerId ) const;

	//	public constant methods
	double getProgress() const;
	const char* getStatus() const;
	//long hasChanged() const;

protected:
	Model();

	//	protected methods
	void fireRefreshEvent( const char* id );
	void setChanged( const char* id );
	void fireChangeEvent( const openocl::base::event::RefreshEvent& aRefreshEvent );
	void setProgress( double progress );
	void setStatus( const char* status );

	//	protected pseudo-constant methods
	void lock( const char* callerId ) const;
	void unlock( const char* callerId ) const;

	openocl::threads::Mutex*      progressMutex;
	openocl::threads::Mutex*      readMutex;
	openocl::threads::Mutex*      writeMutex;
	openocl::threads::EventQueue* eventQueue;
	openocl::util::Sequence*      changeEventListeners;

	double progress;
	const char* status;
	//long changed;
};

};};

#endif

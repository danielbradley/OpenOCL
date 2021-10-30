/*
 *  Copyright (C) 1997-2006 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/FocusManager.h"

#include "openocl/ui/Component.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/helpers/EventHandlerHelper.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::base::event::helpers;
using namespace openocl::ui;
using namespace openocl::util;

//---------------------------------------------------------------------
//	public static methods
//---------------------------------------------------------------------


//---------------------------------------------------------------------
//	constructors
//---------------------------------------------------------------------

FocusManager::FocusManager( const String& name )
{
	this->name = new String( name );
	this->focus = null;
	this->components = new Sequence();
	this->eventHandler = new EventHandlerHelper();
	this->current = -1;
	this->count = 0;
	this->topScope = false;
}

FocusManager::FocusManager( const FocusManager& aFocusManager )
{
	fprintf( stderr, "FocusManager::FocusManager( const FocusManager& ): should not need to clone focus manager, aborting!\n" );
	abort();
}

FocusManager::~FocusManager()
{
	Iterator* it = this->components->iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		Component& com2 = const_cast<Component&>( com );
		com2.removeEventListener( *this );
	}
	delete it;
	delete this->components;
	delete this->eventHandler;
	delete this->name;
}

//---------------------------------------------------------------------
//	public virtual methods (EventListener)
//---------------------------------------------------------------------

void
FocusManager::deliverEvent( const Event& anEvent )
{
	//	A focus manager receives Focus Requests from Components that have
	//	been clicked.
	//	


	unsigned int type = anEvent.getType();
	if ( Event::FOCUS_EVENT == type )
	{
		const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
		const Component& com = dynamic_cast<const Component&>( fe.getEventSource() );
		if ( this->components->contains( com ) )
		{
			FocusEvent circulate_forward( *this, 0, FocusEvent::CIRCULATE_FORWARD );
			FocusEvent circulate_backward( *this, 0, FocusEvent::CIRCULATE_BACKWARD );
			//  int i = this->components->getIndexOf( com );

			if ( FocusEvent::FOCUS_REQUESTED == fe.getFocusEventType() )
			{
				//fprintf( stderr, "FocusManager::deliverEvent: FOCUS_REQUESTED: %i\n", i );
				FocusEvent focus_event( *this, 0, FocusEvent::FOCUS_REQUESTED );
				this->fireEvent( focus_event );
				this->focus = &const_cast<Component&>( dynamic_cast<const Component&>( fe.getEventSource() ) );
				this->refocus();
				this->focus->forwardEvent( circulate_forward );
			}
			else if ( FocusEvent::CIRCULATE_FORWARD == fe.getFocusEventType() )
			{
				//fprintf( stderr, "FocusManager::deliverEvent: CIRCULATE_FORWARD\n" );
				if ( false == this->circulateForward() )
				{
					if ( this->topScope )
					{
						//fprintf( stderr, "FocusManager::deliverEvent: CLOCKED\n" );
						this->circulateForward();
					} else {
						//fprintf( stderr, "FocusManager::deliverEvent: FINISHED\n" );
						this->fireEvent( circulate_forward );
					}
				}
			}
			else if ( FocusEvent::CIRCULATE_BACKWARD == fe.getFocusEventType() )
			{
				//fprintf( stderr, "FocusManager::deliverEvent: CIRCULATE_FORWARD\n" );
				if ( false == this->circulateBackward() )
				{
					if ( this->topScope )
					{
						//fprintf( stderr, "FocusManager::deliverEvent: CLOCKED\n" );
						this->circulateBackward();
					} else {
						//fprintf( stderr, "FocusManager::deliverEvent: FINISHED\n" );
						this->fireEvent( circulate_backward );
					}
				}
			}
		}
	}
}

//---------------------------------------------------------------------
//	public virtual methods (EventHandler)
//---------------------------------------------------------------------

void
FocusManager::fireEvent( const Event& anEvent )
{
	this->eventHandler->fireEvent( anEvent );
}

void
FocusManager::addEventListener( EventListener& anEventListener )
{
	this->eventHandler->addEventListener( anEventListener );
}

void
FocusManager::removeEventListener( EventListener& anEventListener )
{
	this->eventHandler->removeEventListener( anEventListener );
}

//---------------------------------------------------------------------
//	public virtual methods (Component)
//---------------------------------------------------------------------

void
FocusManager::forwardEvent( const Event& anEvent )
{
#ifdef DEBUG_OPENOCL_UI_FOCUSMANAGER
	fprintf( stderr, "%s.FocusManager::forwardEvent()\n", this->name->getChars() );
#endif
	mask event_type = anEvent.getType();

	if ( Event::KEY_EVENT == (Event::KEY_EVENT & event_type) )
	{
		if ( this->focus )
		{
			this->focus->forwardEvent( anEvent );
		}
	}
	else if ( Event::FOCUS_EVENT == (Event::FOCUS_EVENT & event_type) )
	{
		const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
		int focus_type = fe.getFocusEventType();

		if ( FocusEvent::CIRCULATE_FORWARD == focus_type )
		{
			if ( false == this->circulateForward() )
			{
				this->fireEvent( anEvent );
			}
		}
		else if ( FocusEvent::CIRCULATE_BACKWARD == focus_type )
		{
			if ( false == this->circulateBackward() )
			{
				this->fireEvent( anEvent );
			}
		}
		else if ( this->focus )
		{
			this->focus->forwardEvent( anEvent );

			const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
			if ( FocusEvent::LOSE_FOCUS == fe.getFocusEventType() )
			{
				this->focus = null;
				this->current = -1;
			}
		}
	}
	else
	{
		unsigned int max = this->components->getLength();
		for ( unsigned int i=0; i < max; i++ )
		{
			dynamic_cast<Component&>( this->components->elementAt( i ) ).forwardEvent( anEvent );
		}
	}
}

//---------------------------------------------------------------------
//	public methods
//---------------------------------------------------------------------

void
FocusManager::addComponent( Component& aComponent )
{
	this->components->add( aComponent );
	aComponent.addEventListener( *this );
	this->count++;

	//if ( !this->focus )
	//{
	//	this->setFocus( aComponent );
	//}
}

bool
FocusManager::circulateForward()
{
	//fprintf( stderr, "FocusManager::circulateForward: initial current: %i\n", this->current );
	bool circulated = false;
	
	int max = this->count;
	this->current++;
	if ( this->current < max )
	{
		//fprintf( stderr, "FocusManager::circulateForward: changed current: %i\n", this->current );
		FocusEvent take_focus( *this, 0, FocusEvent::CIRCULATE_FORWARD );

		Object& obj = this->components->elementAt( this->current );
		Component* com = &dynamic_cast<Component&>( obj );
		this->focus = com;
		this->focus->forwardEvent( take_focus );
		this->refocus();
		circulated = true;
	} else {
		this->focus = null;
		this->current = -1;
		this->refocus();
	}
	//fprintf( stderr, "FocusManager::circulateForward: leaving current: %i\n", this->current );
	return circulated;
}

bool
FocusManager::circulateBackward()
{
	bool circulated = false;
	FocusEvent take_focus( *this, 0, FocusEvent::CIRCULATE_BACKWARD );
	
	int max = this->count;
	if ( 0 < this->current )
	{

		this->current--;
		Object& obj = this->components->elementAt( this->current );
		Component* com = &dynamic_cast<Component&>( obj );
		this->focus = com;
		this->focus->forwardEvent( take_focus );
		this->refocus();
		circulated = true;
	}
	else if ( -1 == this->current )
	{
		this->current = max - 1;
		Object& obj = this->components->elementAt( this->current );
		Component* com = &dynamic_cast<Component&>( obj );
		this->focus = com;
		this->focus->forwardEvent( take_focus );
		this->refocus();
		circulated = true;
	}
	else
	{
		this->focus = null;
		this->current = -1;
		this->refocus();
	}
	return circulated;
}

void
FocusManager::setAsTopScope()
{
	this->topScope = true;
}

void
FocusManager::setFocus( Component& aComponent )
{
	int count = this->count;
	int i;
	for ( i=0; i < count; i++ )
	{
		const Object& obj = this->components->elementAt( i );
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( obj ) );
		if ( &com == &aComponent )
		{
			this->current = i;
			break;
		}
	}
	if ( this->current != i )
	{
		this->addComponent( aComponent );
		this->current = i;
	}
	this->focus = &aComponent;
}

void
FocusManager::purge()
{
	Iterator* it = this->components->iterator();
	while ( it->hasNext() )
	{
		const Component& com = dynamic_cast<const Component&>( it->next() );
		Component& com2 = const_cast<Component&>( com );
		com2.removeEventListener( *this );
	}
	delete it;
	this->components->removeAllElements();
	this->focus = null;
	this->current = 0;
	this->count = 0;
}

const Sequence&
FocusManager::getComponents() const
{
	return *this->components;
}

//---------------------------------------------------------------------
//	private methods
//---------------------------------------------------------------------

void
FocusManager::refocus()
{
	Component* current_focus = this->focus;
	int count = this->count;
	
	FocusEvent lose_focus( *this, 0, FocusEvent::LOSE_FOCUS );

	for ( int i=0; i < count; i++ )
	{
		const Object& obj = this->components->elementAt( i );
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( obj ) );
		if ( &com == current_focus )
		{
			this->current = i;
		} else {
			com.forwardEvent( lose_focus );
		}
	}
}
		
		

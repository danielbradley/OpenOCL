/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/carbon/CarbonEventManager.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/EventDispatcher.h>
#include <openocl/util/Sequence.h>

#include <carbon/carbon.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;
using namespace openocl::ui::carbon;
using namespace openocl::util;

CarbonEventManager* CarbonEventManager::eventManager;

CarbonEventManager::CarbonEventManager()
{
	this->dispatchers = new Sequence();
	CarbonEventManager::eventManager = this;
}

CarbonEventManager::~CarbonEventManager()
{}

void
CarbonEventManager::registerDispatcher( unsigned long int id, EventDispatcher& anEventDispatcher )
{
	this->dispatchers->add( dynamic_cast<Object*>( &anEventDispatcher ) );
}

void
CarbonEventManager::postEvent( Event* anEvent )
{
	unsigned long int target = anEvent->getTargetId();

	Iterator* it = this->dispatchers->iterator();
	while ( it->hasNext() )
	{
		EventDispatcher& e = const_cast<EventDispatcher&>( dynamic_cast<const EventDispatcher&>( it->next() ) );
		if ( e.getEventDispatcherId() == target )
		{
			e.dispatchEvent( anEvent->copy() );
		}
	}

	delete anEvent;
}

void
CarbonEventManager::enterEventLoop( bool* loop, unsigned long winId )
{
	EventRef       theEvent;
	EventTargetRef theTarget = GetEventDispatcherTarget();

	unsigned int event_nr = 0;
	int event_class;
	int event_kind;

	while ( loop && (noErr == ReceiveNextEvent( 0, NULL, kEventDurationForever, true, &theEvent )) )
	{
		event_class = GetEventClass( theEvent );
		event_kind = GetEventKind( theEvent );
		
		switch ( event_class )
		{
		case kEventClassMouse:
			//fprintf( stderr, "MouseEvent: " );
			//switch ( event_kind )
			//{
			//default:
			//	fprintf( stderr, "kind( %i )\n", event_kind );
			//}
			break;
		case kEventClassKeyboard:
			fprintf( stderr, "Keyboard event\n" );
			break;
		case kEventClassTextInput:
			fprintf( stderr, "TextInput event\n" );
			break;			
		case kEventClassApplication:
			fprintf( stderr, "Application event: " );
			switch ( event_kind )
			{
			case kEventAppActivated:
				fprintf( stderr, "application activated (gained focus)\n" );
				break;
			case kEventAppDeactivated:
				fprintf( stderr, "application deactivated (lost focus)\n" );
				break;
			case kEventAppQuit:
				fprintf( stderr, "application quit\n" );
				loop = false;
				break;
			case kEventAppLaunchNotification:
				fprintf( stderr, "application launch notification\n" );
				break;
			case kEventAppLaunched:
				fprintf( stderr, "application launched\n" );
				break;
			case kEventAppTerminated:
				fprintf( stderr, "application terminated\n" );
				break;
			case kEventAppFrontSwitched:
				fprintf( stderr, "application front switched\n" );
				break;
			case kEventAppFocusMenuBar:
				fprintf( stderr, "application focus menu bar\n" );
				break;
			case kEventAppFocusNextDocumentWindow:
				fprintf( stderr, "application focus next document window\n" );
				break;
			case kEventAppFocusNextFloatingWindow:
				fprintf( stderr, "application focus next document window\n" );
				break;
			case kEventAppFocusToolbar:
				fprintf( stderr, "application focus toolbar \n" );
				break;
			case kEventAppGetDockTileMenu:
				fprintf( stderr, "application get dock tile menu\n" );
				break;
			case kEventAppHidden:
				fprintf( stderr, "application hidden\n" );
				break;
			case kEventAppShown:
				fprintf( stderr, "application shown\n" );
				break;
			case kEventAppSystemUIModeChanged:
				fprintf( stderr, "application system UI mode changed\n" );
				break;
			default:
				fprintf( stderr, "unknown\n" );
			}
			break;
		case kEventClassAppleEvent:
			fprintf( stderr, "AppleEvent event\n" );
			break;
		case kEventClassMenu:
			fprintf( stderr, "Menu event\n" );
			break;
		case kEventClassWindow:
			fprintf( stderr, "Window event: " );
			switch ( event_kind )
			{
			case kEventWindowUpdate:
				fprintf( stderr, "kEventWindowUpdate\n" );
				break;
			default:
				fprintf( stderr, "unknown\n" );
			}
			break;
		case kEventClassControl:
			fprintf( stderr, "Control event: " );
			switch( event_kind )
			{
			case kEventControlDraw:
				fprintf( stderr, "kEventControlDraw\n" );
				break;
			default:
				fprintf( stderr, "unknown\n" );
			}
			break;
		case kEventClassCommand:
			fprintf( stderr, "Command event\n" );
			break;
		case kEventClassTablet:
			fprintf( stderr, "Tablet event\n" );
			break;
		case kEventClassVolume:
			fprintf( stderr, "Volume event\n" );
			break;
		case kEventClassAppearance:
			fprintf( stderr, "Appearance event\n" );
			break;
		case kEventClassService:
			fprintf( stderr, "Service event\n" );
			break;
		case kEventClassToolbar:
			fprintf( stderr, "Toolbar event\n" );
			break;
		case kEventClassToolbarItem:
			fprintf( stderr, "ToolbarItem event\n" );
			break;
		case kEventClassAccessibility:
			fprintf( stderr, "Unhandled event (%x)\n", event_class );
			break;
		case 1667724064:	//	Unknown event class
		case 63677320: 
		default:
			fprintf( stderr, "Undocumented event (%x)\n", event_class );
		}

		SendEventToEventTarget (theEvent, theTarget);
		ReleaseEvent(theEvent);
		event_nr++;
	}
}

OSStatus
CarbonDrawHandler( EventHandlerCallRef handler, EventRef event, void* userData )
{
	fprintf( stderr, "CarbonDrawHandler()\n" );
	return 0;
}


 

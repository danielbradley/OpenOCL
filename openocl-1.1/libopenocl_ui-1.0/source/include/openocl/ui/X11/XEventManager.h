/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XEVENTMANAGER_H
#define OPENOCL_UI_X11_XEVENTMANAGER_H

#include <openocl/base/Object.h>
//#include <openocl/mvc/Controller.h>
#include <openocl/base/event/EventSource.h>

#include <openocl/threads.h>
#include <openocl/util.h>
#include "openocl/ui.h"
#include "openocl/ui/X11.h"

namespace openocl {
  namespace ui {
    namespace X11 {

/**
 *  The Event Manager class plays two roles; it acts as a Controller object
 *  in a typical Model View Controller style application, so user classes
 *  register as Listeners of the Controller.
 *
 *  It also acts as a event relay mechanism for events destined for particular
 *  window (Panel) objects.
 *
 *  In the first case the mechanism is arbitrary; it does not care who has registered
 *  as a listener, however in the second case the object checks the EventHandler Id
 *  of the registered class.
 */

class XEventManager : public openocl::base::Object, // openocl::mvc::Controller,
                      public openocl::base::event::EventSource
{
public:
	static const mask SHIFT = 0x00000001; // (1)  Shift
	static const mask CTRL  = 0x00000004; // (4)  Ctrl
	static const mask APPLE = 0x00000010; // (16) Apple??

  static const unsigned int EXPOSURE;
  static const unsigned int KEY_PRESS;
  static const unsigned int BUTTON_PRESS;
  static const unsigned int STRUCTURE_NOTIFY;

  XEventManager( void* xdisplay );
  ~XEventManager();

  //void selectEvent( const XDisplay& aDisplay,
  //                  const Frame& aFrame,
  //                  flags eventTypes );

  // EventSource method
  void registerDispatcher( void* id, openocl::base::event::EventDispatcher& anEventDispatcher );
  void deregisterDispatcher( void* id );


  // EventSource method
  void postEvent( openocl::base::event::Event* anEvent );

  void enterEventLoop( bool* visible, unsigned long winId );

	static void SendCustomEvent( void* display, long winId, mask eventType );

	static void SendEvent( void* display, long winId, bool propagate, long eventMask, void* xevent );


protected:
  openocl::base::event::Event* processMousePress( void* anEvent ); 
  openocl::base::event::Event* processMouseRelease( void* anEvent ); 
  openocl::base::event::Event* processKeyPress( void* anEvent, int press );
  openocl::base::event::Event* processExpose( void* anEvent ); 
  openocl::base::event::Event* processReposition( void* anEvent );
  openocl::base::event::Event* processResize( void* anEvent );
  openocl::base::event::Event* processMotion( void* anEvent ); 
  openocl::base::event::Event* processFocus( void* anEvent );
  openocl::base::event::Event* processClientMessage( void* anEvent, bool* visible );

private:
  void*                      xdisplay;
  openocl::util::Dictionary* dispatchers;
  openocl::util::Sequence* handlers;

  unsigned int countKeyPress;
  unsigned int countButtonPress;
  unsigned int countButtonRelease;
  unsigned int countMotion;
  unsigned int countExpose;
  unsigned int countConfigure;
  unsigned int countFocus;

	static openocl::threads::Mutex mutex;
};

};};};

#endif

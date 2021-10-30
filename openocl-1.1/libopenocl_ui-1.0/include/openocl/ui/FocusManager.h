/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_FOCUSMANAGER_H
#define OPENOCL_UI_FOCUSMANAGER_H

#include "openocl/ui.h"
#include <openocl/base/Object.h>
#include <openocl/base/event/EventHandler.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>
#include <openocl/base/event/helpers.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class FocusManager : public openocl::base::Object,
                     public openocl::base::event::EventListener,
		     public openocl::base::event::EventHandler
{
public:
	static const char* REQUEST_FOCUS;
	static const char* RELEASE_FOCUS;
	static const char* TAKE_FOCUS;

	FocusManager( const openocl::base::String& name );
	explicit FocusManager::FocusManager( const FocusManager& aFocusManager );
	~FocusManager();

	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (EventHandler)
	virtual void fireEvent( const openocl::base::event::Event& anEvent );
	virtual void addEventListener( openocl::base::event::EventListener& anEventListener );
	virtual void removeEventListener( openocl::base::event::EventListener& anEventListener );

	//	public methods
	void addComponent( Component& aComponent );
	bool circulateForward();
	bool circulateBackward();
	void forwardEvent( const openocl::base::event::Event& anEvent );
	void setAsTopScope();
	void setFocus( Component& aComponent );
	void purge();

	//	public constant methods
	const openocl::util::Sequence& getComponents() const;

private:
	void refocus();

	Component*   focus;
	openocl::util::Sequence*    components;
	openocl::base::event::helpers::EventHandlerHelper* eventHandler;
	int current;
	unsigned int count;
	openocl::base::String* name;
	bool topScope;
};

};};

#endif

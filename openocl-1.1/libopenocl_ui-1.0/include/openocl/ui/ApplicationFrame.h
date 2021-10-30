/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_APPLICATIONFRAME_H
#define OPENOCL_UI_APPLICATIONFRAME_H

#include <openocl/base/event/EventListener.h>
#include <openocl/ui/Frame.h>
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class ApplicationFrame : public Frame
{
public:
	//ApplicationFrame( Visual& aVisual );
	ApplicationFrame( Visual& aVisual, bool* loop );
	virtual ~ApplicationFrame();

	//	public virtual method (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );
	
private:
	openocl::ui::Visual& visual;
	bool* loopPtr;
};

};};

#endif

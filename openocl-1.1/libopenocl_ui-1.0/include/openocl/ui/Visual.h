/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_VISUAL_H
#define OPENOCL_UI_VISUAL_H

#include "openocl/ui.h"
#include <openocl/base/event.h>
#include <openocl/base/Interface.h>
//#include <openocl/mvc.h>

namespace openocl {
  namespace ui {

/**
 *  A Visual class encapsulates information about a
 *  Display, the screen number, and any framebuffer information
 */
class Visual : public openocl::base::Interface
{
public:
	static Visual* getDefaultInstance();
	
	//	@deprecated
	static Visual* getInstance();
	virtual ~Visual();

	//virtual openocl::mvc::Controller& getController() = 0;
	virtual openocl::base::event::EventSource& getEventSource() const = 0;
	virtual FontManager&           getFontManager() const = 0;
	virtual OffscreenImageManager& getOffscreenImageManager() const = 0;

	virtual Screen& getDefaultScreen() = 0;

	virtual ApplicationMenu* createApplicationMenu() const = 0;

	virtual void enterEventLoop( bool* visible ) const = 0;
	virtual void enterEventLoop( bool* visible, unsigned long winId ) const = 0;

	//	@deprecated
	//virtual const AbstractDisplay& getDefaultDisplay() const = 0;
	//virtual const AbstractDisplay& getDisplay() const = 0;


protected:
	Visual();

};

};};

#endif


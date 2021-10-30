/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINVISUAL_H
#define OPENOCL_UI_WIN32_WINVISUAL_H

#include "openocl/ui/Visual.h"
#include "openocl/ui.h"
#include "openocl/ui/win32.h"
#include <openocl/base/event.h>
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class WinVisual : public openocl::base::Object, public openocl::ui::Visual
{
friend class WinPanel;
public:
	static WinVisual* instance;

	WinVisual();
	virtual ~WinVisual();

	//	public virtual methods (Visual)
	virtual Screen& getDefaultScreen();
	virtual Panel* createPanel( flags type );
	//virtual openocl::mvc::Controller& getController();

	//	public virtual constant methods (Visual)
	virtual FontManager&           getFontManager() const;
	virtual OffscreenImageManager& getOffscreenImageManager() const;
	virtual openocl::base::event::EventSource& getEventSource() const;
	virtual void enterEventLoop( bool* visible ) const;
	virtual void enterEventLoop( bool* visible, unsigned long winId ) const;

	virtual ApplicationMenu* createApplicationMenu() const;

private:
	void*                  hInstance;
	FontManager*           fontManager;
	OffscreenImageManager* imageManager;
	WinEventManager*       eventManager;
	WinScreen*             defaultScreen;


	long windowsUsed;	
	long buffersUsed;	
	//WinDisplay* windisplay;
};

};};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XVISUAL_H
#define OPENOCL_UI_X11_XVISUAL_H

#include "openocl/ui/Visual.h"
#include "openocl/ui.h"
#include "openocl/ui/X11.h"
#include <openocl/base/event.h>
#include <openocl/base/Object.h>

namespace openocl {
  namespace ui {
    namespace X11 {

/**
 *  A Visual (context) reflects the visual context of a 
 *  parent object.
 */
class XVisual : public openocl::base::Object,
                public openocl::ui::Visual
{
public:
	XVisual();
	virtual ~XVisual();

	//	public virtual methods (Visual)
	virtual Screen& getDefaultScreen();
	//virtual openocl::mvc::Controller& getController();
	
	//	public virtual constant methods (Visual)
	virtual openocl::base::event::EventSource& getEventSource() const;
	virtual FontManager& getFontManager() const;
	virtual OffscreenImageManager& getOffscreenImageManager() const;
	virtual void enterEventLoop( bool* visible ) const;
	virtual void enterEventLoop( bool* visible, unsigned long winId ) const;

	//	public constant methods
	void* getXDisplay() const;

	ApplicationMenu* createApplicationMenu() const;

private:
	void* xdisplay;
	XScreen* xscreen;
	FontManager*           fontManager;
	OffscreenImageManager* imageManager;
	XEventManager*         eventManager;

	unsigned long rootWindowId;
};

};};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_X11_XSCREEN_H
#define OPENOCL_UI_X11_XSCREEN_H

#include "openocl/ui.h"
#include "openocl/ui/X11.h"
#include "openocl/ui/Dimensions.h"
#include "openocl/ui/Screen.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {
		namespace X11 {

class XScreen : public openocl::base::Object, public openocl::ui::Screen
{
public:
	XScreen( XVisual& aVisual, unsigned int aScreenId );
	virtual ~XScreen();

	//	public virtual methods (Screen)
	Panel* createPanel( int type );
	
	//	public virtual constant methods (Screen)
	Dimensions	getScreenDimensions() const;
	unsigned long	getScreenDepth() const;
	unsigned long	getScreenId() const;

	//	public methods
	XVisual&	getXVisual() const;
	void*           getXDisplay() const;
	unsigned long   getRootWindowId() const;

private:
	XVisual&      xvisual;
	unsigned int  screenId;

	void*         xdisplay;
	unsigned long rootWindowId;
	Dimensions    screenDimensions;
	unsigned long depth;

};

};};};

#endif

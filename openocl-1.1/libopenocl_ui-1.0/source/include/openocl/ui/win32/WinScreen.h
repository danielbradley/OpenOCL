/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WIN32_WINSCREEN_H
#define OPENOCL_UI_WIN32_WINSCREEN_H

#include "openocl/ui/Screen.h"
#include "openocl/ui.h"
#include "openocl/ui/win32.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {
		namespace win32 {

class WinScreen : public openocl::base::Object, public openocl::ui::Screen
{
public:
	WinScreen( WinVisual& aVisual, unsigned int aScreenId );

	//	public virtual methods (Screen)
	virtual Panel* createPanel( int type );

	//	public virtual constant methods (Screen)
	virtual Dimensions    getScreenDimensions() const;
	virtual unsigned long getScreenDepth() const;
	virtual unsigned long getScreenId() const;

	//	public methods
	WinVisual&	getWinVisual() const;

private:
	WinVisual& winvisual;
	unsigned int screenId;

	int clientX;
	int clientY;
	Dimensions clientScreen;
	Dimensions primaryScreen;
	Dimensions virtualScreen;
};

};};};

#endif

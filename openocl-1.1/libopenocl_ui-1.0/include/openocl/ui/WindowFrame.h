/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_WINDOWFRAME_H
#define OPENOCL_UI_WINDOWFRAME_H

#include <openocl/ui/Frame.h>
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class WindowFrame : public Frame
{
public:
	WindowFrame( Visual& aVisual );
	virtual ~WindowFrame();

};

};};

#endif

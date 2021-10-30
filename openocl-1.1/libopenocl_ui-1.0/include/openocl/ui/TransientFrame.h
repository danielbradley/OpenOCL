/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TRANSIENTFRAME_H
#define OPENOCL_UI_TRANSIENTFRAME_H

#include "openocl/ui/FloatingFrame.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>

namespace openocl {
	namespace ui {

class TransientFrame : public FloatingFrame
{
public:
	TransientFrame( Visual& aVisual );
	
	void deliverEvent( const openocl::base::event::Event& anEvent );
};

};};

#endif

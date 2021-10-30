/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CURSOR_H
#define OPENOCL_UI_CURSOR_H

#include <openocl/base/Interface.h>
#include "openocl/ui.h"

namespace openocl {
  namespace ui {

class Cursor : public openocl::base::Interface
{
public:
	static const int UP_DOWN     = 17;
	static const int LEFT_RIGHT  = 71;
	static const int CROSS_HAIRS = 31;
	static const int MOVE        = 53;
	static const int TEXT_CURSOR = 153;
};

};};

#endif


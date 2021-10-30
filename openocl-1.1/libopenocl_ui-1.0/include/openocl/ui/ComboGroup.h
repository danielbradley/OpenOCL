/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COMBOGROUP_H
#define OPENOCL_UI_COMBOGROUP_H

#include "openocl/ui/OpaqueContainer.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class ComboGroup : public openocl::ui::OpaqueContainer
{
public:
	ComboGroup();
	virtual ~ComboGroup();

	bool draw( GraphicsContext& gc ) const;

private:
};

};};

#endif

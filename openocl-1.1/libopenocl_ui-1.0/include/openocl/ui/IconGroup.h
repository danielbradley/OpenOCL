/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_ICONGROUP_H
#define OPENOCL_UI_ICONGROUP_H

#include "openocl/ui.h"
#include "openocl/ui/Container.h"

namespace openocl {
	namespace ui {

class IconGroup : public openocl::ui::Container
{
public:
	static const mask ARRANGED = 0;
	static const mask FREEFORM = 1;
	static const mask GRAPH    = 2;

	IconGroup();
	IconGroup( flags iconArrangement );
	virtual ~IconGroup();
private:
};

};};

#endif

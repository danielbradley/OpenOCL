/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_OPAQUECONTAINER_H
#define OPENOCL_UI_OPAQUECONTAINER_H

#include "openocl/ui/Container.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/imaging/Color.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class OpaqueContainer : public openocl::ui::Container
{
public:
	OpaqueContainer( const openocl::imaging::Color& backgroundColor );
	virtual ~OpaqueContainer();

	bool draw( GraphicsContext& gc ) const;
	
	const openocl::imaging::Color& getBackgroundColor() const;


private:
	openocl::imaging::Color bgColor;
};

};};

#endif

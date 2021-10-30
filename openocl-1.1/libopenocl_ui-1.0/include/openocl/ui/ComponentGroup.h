/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COMPONENTGROUP_H
#define OPENOCL_UI_COMPONENTGROUP_H

#include "openocl/ui/OpaqueContainer.h"
#include <openocl/imaging/Color.h>

namespace openocl {
	namespace ui {

class ComponentGroup : public openocl::ui::Container
{
public:
	ComponentGroup( const openocl::imaging::Color& aColor );
//	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	bool draw( GraphicsContext& gc ) const;
};

};};

#endif

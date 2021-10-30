/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */
 
#ifndef OPENOCL_UI_GRAPHLAYOUT_H
#define OPENOCL_UI_GRAPHLAYOUT_H

#include "openocl/ui/LayoutManager.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class GraphLayout : public openocl::base::Object, public LayoutManager
{
public:
	GraphLayout();
	virtual ~GraphLayout();

	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
};

};};

#endif


/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_BORDERLAYOUT_H
#define OPENOCL_UI_BORDERLAYOUT_H

#include "openocl/ui.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/base/Object.h>

namespace openocl {
  namespace ui {

class BorderLayout : public openocl::base::Object, public LayoutManager
{
public:
	static const flags TOP    = 0;
	static const flags LEFT   = 1;
	static const flags RIGHT  = 2;
	static const flags BOTTOM = 3;
	static const flags CENTER = 4;

	static const flags NO_STRETCH           = 0x00;
	static const flags STRETCH_HORIZONTALLY = 0x01;
	static const flags STRETCH_VERTICALLY   = 0x10;
	static const flags STRETCH              = 0x11;

	BorderLayout( flags stretch = NO_STRETCH );
	virtual ~BorderLayout();

	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;

private:
	Region* top;
	Region* left;
	Region* right;
	Region* bottom;
	Region* center;

	flags stretch;
};

};};

#endif


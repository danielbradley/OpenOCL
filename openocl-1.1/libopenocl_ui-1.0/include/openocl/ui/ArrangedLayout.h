/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_ARRANGEDLAYOUT_H
#define OPENOCL_UI_ARRANGEDLAYOUT_H

#include "openocl/ui.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class ArrangedLayout : public openocl::base::Object, public LayoutManager
{
public:
	ArrangedLayout();
	virtual ~ArrangedLayout();

	//	public methods
	void                 setItemWidth( unsigned int width );

	//	public virtual methods (LayoutManager)
	virtual void         doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions   preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
};

};};

#endif


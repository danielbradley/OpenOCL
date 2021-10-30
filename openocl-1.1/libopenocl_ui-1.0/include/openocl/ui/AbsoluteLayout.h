/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */
 
#ifndef OPENOCL_UI_ABSOLUTELAYOUT_H
#define OPENOCL_UI_ABSOLUTELAYOUT_H

#include "openocl/ui/LayoutManager.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class AbsoluteLayout : public openocl::base::Object, public LayoutManager
{
public:
	AbsoluteLayout();
	virtual ~AbsoluteLayout();

	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutWidth( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutHeight( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;

};

};};

#endif


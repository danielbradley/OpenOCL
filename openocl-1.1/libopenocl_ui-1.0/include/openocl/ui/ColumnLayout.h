/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_COLUMNLAYOUT_H
#define OPENOCL_UI_COLUMNLAYOUT_H

#include <openocl/base/Object.h>
#include "openocl/ui/LayoutManager.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class ColumnLayout : public openocl::base::Object, public LayoutManager
{
public:
	static const int WEST = 1;
	static const int EAST = 0;

	ColumnLayout();
	ColumnLayout( flags layoutHints );
	virtual ~ColumnLayout();

	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;

	//virtual unsigned int preferredLayoutWidth( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutHeight( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;

private:
	int hints;
};

};};

#endif

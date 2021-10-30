/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */
 
#ifndef OPENOCL_UI_TABLELAYOUT_H
#define OPENOCL_UI_TABLELAYOUT_H

#include "openocl/ui/LayoutManager.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {

class TableLayout : public openocl::base::Object, public LayoutManager
{
public:
	TableLayout( unsigned int nrOfColumns );
	virtual ~TableLayout();

	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
	virtual Dimensions preferredLayoutEastward( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;

private:
	unsigned int  columns;
	unsigned int* minWidths;
	unsigned int  commonHeight;
};

};};

#endif


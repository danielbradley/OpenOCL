/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DIRECTEDLAYOUT_H
#define OPENOCL_UI_DIRECTEDLAYOUT_H

#include "openocl/ui/LayoutManager.h"

#include <openocl/base/Object.h>
#include <openocl/util.h>

namespace openocl {
  namespace ui {

class DirectedLayout : public openocl::base::Object, public LayoutManager
{
public:
	DirectedLayout( flags hints );
	virtual ~DirectedLayout();

	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;

protected:
	virtual void doEastwardLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual void doWestwardLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual void doSouthwardLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual void doNorthwardLayout( Container& aContainer, const GraphicsContext& gc ) const;

private:
	flags layoutHints;
};

};};

#endif

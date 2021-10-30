/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_LAYOUTMANAGER_H
#define OPENOCL_UI_LAYOUTMANAGER_H

#include <openocl/base/Interface.h>
#include "openocl/ui.h"
#include "openocl/ui/Dimensions.h"
#include <openocl/util.h>

namespace openocl {
  namespace ui {

//Abstract
class LayoutManager : public openocl::base::Interface
{
public:
	//	Layout direction
	static const mask DIRECTION = 0x00FF;
	static const mask NORTHWARD = 0x0001;
	static const mask SOUTHWARD = 0x0002;
	static const mask WESTWARD  = 0x0004;
	static const mask EASTWARD  = 0x0008;
	static const mask UPWARD    = 0x0010;
	static const mask DOWNWARD  = 0x0020;

	//	Cell alignment
	static const mask ALIGN     = 0x1F00;
	static const mask LEFT      = 0x0100;
	static const mask CENTERED  = 0x0200;
	static const mask RIGHT     = 0x0400;

	static const mask VALIGN    = 0xF000;
	static const mask EXPANDED  = 0x1000;
	static const mask TOP       = 0x2000;
	static const mask MIDDLE    = 0x4000;
	static const mask BOTTOM    = 0x8000;

	virtual ~LayoutManager();

	/**
	 *  This will usually be called from Container like follows:
	 *
	 *   this->layoutManager->layout( *this, gc );
	 */

	virtual void         doLayout( Container& aContainer, const GraphicsContext& gc ) const = 0;
	virtual Dimensions   preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const = 0;

	//virtual unsigned int preferredLayoutWidth( const openocl::util::Sequence& components, const GraphicsContext& gc ) const = 0;
	//virtual unsigned int preferredLayoutHeight( const openocl::util::Sequence& components, const GraphicsContext& gc ) const = 0;
	//virtual unsigned int preferredLayoutWidth( const Container& aContainer, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutHeight( const Container& aContainer, const GraphicsContext& gc ) const;

	static unsigned int maxWidth( const openocl::util::Sequence& components, const GraphicsContext& gc );
	static unsigned int maxHeight( const openocl::util::Sequence& components, const GraphicsContext& gc );

	static unsigned int sumOfWidths( const openocl::util::Sequence& components, const GraphicsContext& gc );
	static unsigned int sumOfHeights( const openocl::util::Sequence& components, const GraphicsContext& gc );

protected:
  LayoutManager();

};

};};

#endif

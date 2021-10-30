/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_BUTTONGROUP_H
#define OPENOCL_UI_BUTTONGROUP_H

#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class ButtonGroup : public Container, public LayoutManager
{
public:
	static unsigned int padding;
	static unsigned int margin;

	ButtonGroup( flags layoutHints );
	virtual ~ButtonGroup();

	//	public methods
	void doLayout( const GraphicsContext& gc );
	void setSize( unsigned int width, unsigned int height );

	//	public virtual constant methods (LayoutManager)
	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

private:
	flags hints;
	Region* group;
	Region* outline;
	Region* buttons;

	unsigned int width;
	unsigned int height;

};

};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TOOLBARGROUP_H
#define OPENOCL_UI_TOOLBARGROUP_H

#include "openocl/ui.h"
#include "openocl/ui/Component.h"
#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class ToolBarGroup : public openocl::ui::OpaqueContainer  //, public LayoutManager
{
public:
	static unsigned int padding;

	ToolBarGroup();
	virtual ~ToolBarGroup();

	//	virtual public methods
	virtual void doLayout( const GraphicsContext& gc );

	//	public methods
	void setLogoId( const openocl::base::String& anImageId );
	void addToolBar( ToolBar& aToolBar );
	void removeToolBar( ToolBar& aToolBar );
	
	//	public virtual const methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
	Region* border;
	Region* bars;
	Region* logo;

	openocl::ui::Container* toolbars;
	openocl::base::String* logoId;
};

};};

#endif

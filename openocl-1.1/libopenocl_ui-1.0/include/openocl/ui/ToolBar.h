/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TOOLBAR_H
#define OPENOCL_UI_TOOLBAR_H

#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/Dimensions.h>
#include <openocl/ui.h>

namespace openocl {
	namespace ui {

class ToolBar : public openocl::ui::Container
{
public:
	static unsigned int padding;

	ToolBar();
	ToolBar( const openocl::base::String& name );
	virtual ~ToolBar();

	//	public virtual methods (Component)
	virtual void doLayout( const openocl::ui::GraphicsContext& gc );

	//	public methods
	void add( openocl::ui::Icon& anIcon );
	void add( openocl::ui::Icon* anIcon );
	void addIcon( const openocl::base::String& name, const openocl::base::String& actionId, const openocl::base::String& aLabel, const openocl::base::String& imageId, unsigned int width, unsigned int height );

	//	public virtual constant methods (Component)
	virtual openocl::ui::Dimensions getPreferredDimensions( const openocl::ui::GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( openocl::ui::GraphicsContext& gc ) const;	
//	virtual unsigned int getPreferredWidth( const openocl::ui::GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const openocl::ui::GraphicsContext& gc ) const;

	
private:
	openocl::ui::IconGroup* icongroup;

	openocl::ui::Region* icons;
};

};};

#endif

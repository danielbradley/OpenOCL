/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_MENUGROUP_H
#define OPENOCL_UI_MENUGROUP_H

#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/LayoutManager.h"
#include <openocl/imaging.h>
#include <openocl/imaging/Color.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class MenuGroup : public openocl::ui::OpaqueContainer
{
public:
	static const flags HORIZONTAL = 0;
	static const flags VERTICAL   = 1;

	MenuGroup( flags layout = VERTICAL );
	virtual ~MenuGroup();

	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component )
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public methods
	void activateMenuContaining( int x, int y );

	void setOuterShade( const openocl::imaging::Color& shade );

	//	public virtual constant methods
	bool draw( GraphicsContext& gc ) const;

private:
	openocl::imaging::Color shadeColor;
	flags layout;

	bool active;
};

};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_SHORTCUTBOX_H
#define OPENOCL_CONTROLS_SHORTCUTBOX_H

#include "openocl/controls.h"
#include <openocl/io.h>
#include <openocl/ui.h>
#include <openocl/base/event.h>
#include <openocl/ui/Control.h>

namespace openocl {
	namespace controls {

class ShortcutBox : public openocl::ui::Control
{
public:
	ShortcutBox();
	ShortcutBox( const openocl::base::String& location );
	virtual ~ShortcutBox();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public virtual constant methods (Component)
	virtual bool draw( openocl::ui::GraphicsContext& gc ) const;	

private:
	//void createIcons( const openocl::io::Path& path );
	//void loadFile( const openocl::io::Path& path );

	openocl::ui::Container* base;
};

};};

#endif

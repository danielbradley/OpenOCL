/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_VALUESELECTOR_H
#define OPENOCL_CONTROLS_VALUESELECTOR_H

#include "openocl/controls.h"
#include <openocl/data.h>
#include <openocl/io.h>
#include <openocl/ui.h>
#include <openocl/base/event.h>
#include <openocl/ui/Control.h>

namespace openocl {
	namespace controls {

class ValueSelector : public openocl::ui::Control
{
public:
	ValueSelector( openocl::data::TableData& data, const openocl::base::String& fieldName );
	virtual ~ValueSelector();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public methods
	void refresh();

	//	public virtual constant methods (Component)
	//virtual bool draw( openocl::ui::GraphicsContext& gc ) const;	

private:
	openocl::ui::Container* base;
};

};};

#endif

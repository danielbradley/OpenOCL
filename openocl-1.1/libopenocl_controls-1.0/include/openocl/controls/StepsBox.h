/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_STEPSBOX_H
#define OPENOCL_CONTROLS_STEPSBOX_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/ui/Control.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class StepsBox : public openocl::ui::Control
{
public:
	StepsBox( openocl::util::Sequence& stepsContainers );
	virtual ~StepsBox();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

private:
	openocl::ui::Container* base;
};

};};

#endif

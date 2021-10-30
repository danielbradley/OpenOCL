/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_WARNINGBOX_H
#define OPENOCL_CONTROLS_WARNINGBOX_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/ui/Control.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class WarningBox : public openocl::ui::Control
{
public:
	//WarningBox( const openocl::base::String& button1 );
	WarningBox( const openocl::base::String& message, const openocl::base::String& button1, const openocl::base::String& button2 );
	WarningBox( const openocl::base::String& message, const openocl::base::String& button1, const openocl::base::String& button2, const openocl::base::String& button3 );
	virtual ~WarningBox();

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public constant methods (Control)
	int getLastButtonPressed() const;
private:
	openocl::ui::Container* base;

};

};};

#endif

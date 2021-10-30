/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CARBON_CARBONDISPLAY_H
#define OPENOCL_UI_CARBON_CARBONDISPLAY_H

#include "openocl/ui/AbstractDisplay.h"
#include "openocl/ui/carbon.h"
#include <openocl/base/event.h>
#include <openocl/mvc.h>

namespace openocl {
	namespace ui {
		namespace carbon {

class CarbonDisplay : public openocl::ui::AbstractDisplay
{
public:
	CarbonDisplay();
	virtual ~CarbonDisplay();

	virtual Panel* createPanel( flags type ) const;

	
	virtual openocl::base::event::EventSource& getEventSource() const;

	virtual openocl::mvc::Controller& getController() const;

	virtual unsigned int getDefaultScreen() const;
	virtual unsigned int getWidthOf( unsigned int screenNum ) const;
	virtual unsigned int getHeightOf( unsigned int screenNum ) const;
	virtual unsigned int getWidthMMOf( unsigned int screenNum ) const;
	virtual unsigned int getHeightMMOf( unsigned int screenNum ) const;

	void enterEventLoop( bool* run, unsigned long winId ) const;

private:
	CarbonEventManager* eventManager;

};

};};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CARBON_CARBONVISUAL_H
#define OPENOCL_UI_CARBON_CARBONVISUAL_H

#include "openocl/ui/Visual.h"
#include "openocl/ui.h"
#include "openocl/ui/carbon.h"
#include <openocl/base/Object.h>

namespace openocl {
	namespace ui {
		namespace carbon {

class CarbonVisual : public openocl::base::Object, public openocl::ui::Visual
{
public:
	CarbonVisual();
	virtual ~CarbonVisual();

	// Visual Interface Method
	ApplicationMenu* createApplicationMenu() const;

	Panel* createPanel( flags type ) const;

	void enterEventLoop( bool* visible ) const;
	void enterEventLoop( bool* visible, unsigned long winId ) const;
	virtual const AbstractDisplay& getDisplay() const;
	
	const CarbonDisplay& getCarbonDisplay() const;

private:
	CarbonDisplay* display;

};

};};};

#endif

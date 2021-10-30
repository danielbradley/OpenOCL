/*
 *  Copyright (c) 2005 Daniel Robert Bradley.    All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_APPLICATION_DIALOGMANAGER_H
#define OPENOCL_APPLICATION_DIALOGMANAGER_H

#include <openocl/base/Object.h>
#include <openocl/application.h>
#include <openocl/ui.h>

namespace openocl {
	namespace application {

class DialogManager : public openocl::base::Object
{
public:
	static void showDialog( openocl::ui::Panel& aPanel, openocl::ui::Component& com );
	static openocl::base::String* openDialog( openocl::ui::Panel& aPanel );
	
};

};};

#endif

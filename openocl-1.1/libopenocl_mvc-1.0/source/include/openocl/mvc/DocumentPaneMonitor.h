/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MVC_DOCUMENTPANEMONITOR_H
#define OPENOCL_MVC_DOCUMENTPANEMONITOR_H

#include <openocl/threads.h>
#include <openocl/mvc.h>
#include <openocl/ui.h>
#include <openocl/ui/DocumentPane.h>

namespace openocl {
	namespace mvc {

class DocumentPaneMonitor : public openocl::ui::DocumentPane
{
public:
	DocumentPaneMonitor();
	~DocumentPaneMonitor();

	virtual void doLayout( const openocl::ui::GraphicsContext& gc );
	
	void setNextComponent( openocl::ui::Component& com );
	void releaseComponent( openocl::ui::Component& com );

private:
	openocl::threads::Mutex* nextMutex;
	openocl::ui::Component* next;
	bool refreshed;

};

};};

#endif

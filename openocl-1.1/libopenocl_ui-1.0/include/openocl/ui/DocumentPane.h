/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DOCUMENTPANE_H
#define OPENOCL_UI_DOCUMENTPANE_H

#include "openocl/ui/Pane.h"
#include "openocl/ui.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class DocumentPane : public Pane
{
public:
	static unsigned int padding;

	DocumentPane();
	virtual ~DocumentPane();

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;

private:
	Region* bounds;
	
	
	
};

};};

#endif

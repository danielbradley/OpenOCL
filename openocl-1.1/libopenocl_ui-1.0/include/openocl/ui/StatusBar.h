/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_STATUSBAR_H
#define OPENOCL_UI_STATUSBAR_H

#include "openocl/ui.h"
#include "openocl/ui/OpaqueContainer.h"
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class StatusBar : public openocl::ui::OpaqueContainer
{
public:
	StatusBar();
	virtual ~StatusBar();

	//	virtual public methods

	//	public virtual const methods (Component)
	//virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
};

};};

#endif

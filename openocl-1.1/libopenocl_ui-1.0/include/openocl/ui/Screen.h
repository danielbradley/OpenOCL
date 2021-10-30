/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SCREEN_H
#define OPENOCL_UI_SCREEN_H

#include "openocl/ui.h"
#include "openocl/ui/Dimensions.h"
#include <openocl/base/Interface.h>

namespace openocl {
  namespace ui {

class Screen : public openocl::base::Interface
{
public:
	virtual ~Screen();

	//	public abstract virtual methods (Screen)
	virtual Panel* createPanel( int type ) = 0;

	//	public abstract virtual constant methods (Screen)
	virtual Dimensions    getScreenDimensions() const = 0;
	virtual unsigned long getScreenDepth() const = 0;
	virtual unsigned long getScreenId() const = 0;

};

};};

#endif


/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_RUNTIMESUMMARYITEM_H
#define OPENOCL_UI_RUNTIMESUMMARYITEM_H

#include "openocl/ui.h"
#include "openocl/ui/MenuItem.h"
#include <openocl/base.h>
#include <openocl/base/event.h>

namespace openocl {
	namespace ui {

class RuntimeSummaryItem : public openocl::ui::MenuItem
{
public:
	RuntimeSummaryItem();
	~RuntimeSummaryItem();

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
	long long int* values;
	int last;
	bool clocked;

	long long int objects;
	long long int news;
	long long int cmem;

	bool reset;
};

};};

#endif

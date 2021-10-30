/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_DRAGTAB_H
#define OPENOCL_UI_DRAGTAB_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>

namespace openocl {
	namespace ui {

class DragTab : public openocl::ui::Component
{
public:
	DragTab();
	~DragTab();

	virtual void processEvent( const openocl::base::event::Event& anEvent );
	bool draw( GraphicsContext& gc ) const;

	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	unsigned int getPreferredHeight( const GraphicsContext& gc ) const;

private:
	void initDragTab( GraphicsContext& gc );
	
	OffscreenImage* dragTab;
};

};};

#endif

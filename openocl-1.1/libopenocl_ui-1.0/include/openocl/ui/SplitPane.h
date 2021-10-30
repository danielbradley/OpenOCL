/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SPLITPANE_H
#define OPENOCL_UI_SPLITPANE_H

#include "openocl/ui/Pane.h"
#include "openocl/ui.h"
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class SplitPane : public Pane
{
public:
	SplitPane();
	SplitPane( unsigned int width, unsigned int height );
	virtual ~SplitPane();

	virtual void setName( const openocl::base::String& name );

	virtual void doLayout( const GraphicsContext& gc );

	//bool draw( GraphicsContext& gc ) const;
	
	void processEvent( const openocl::base::event::Event& anEvent );

private:
	unsigned int offsetX;
	unsigned int offsetY;

	unsigned int split;


};

};};

#endif

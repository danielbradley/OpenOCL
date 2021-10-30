/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_BUFFEREDSCROLLER_H
#define OPENOCL_UI_BUFFEREDSCROLLER_H

#include "openocl/ui/Scroller.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class BufferedScroller : public Scroller
{
public:
	BufferedScroller( Component& aComponent, unsigned int width, unsigned int height );
	virtual ~BufferedScroller();

	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );
	virtual bool draw( GraphicsContext& gc ) const;
private:
	Component* com;
	OffscreenImage* buffer;
	unsigned int bufferWidth;
	unsigned int bufferHeight;
	bool         redrawAll;

	Region* bufferedRegion;
};

};};

#endif

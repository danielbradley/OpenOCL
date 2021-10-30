/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SCROLLINGCANVAS_H
#define OPENOCL_UI_SCROLLINGCANVAS_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>

namespace openocl {
  namespace ui {

class ScrollingCanvas : public openocl::ui::Component
{
public:
	ScrollingCanvas();
	~ScrollingCanvas();

	//	public virtual methods (Component)
	virtual void setContainingPanel( Panel& aPanel );
	virtual void setBounds( int anX, int aY, unsigned int aWidth, unsigned int aHeight );
	virtual void doLayout( const GraphicsContext& gc );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Canvas)
	virtual void doCanvasLayout( const GraphicsContext& gc );
	virtual void processCanvasEvent( const openocl::base::event::Event& anEvent );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

	//	public virtual constant methods (Canvas)
	virtual Dimensions getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const = 0;
	virtual bool drawCanvas( GraphicsContext& gc ) const = 0;

private:
	openocl::ui::ScrollPane* scrollPane;
	int caller;

};

};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PANELSCROLLPANE_H
#define OPENOCL_UI_PANELSCROLLPANE_H

#include "openocl/ui/Pane.h"
#include "openocl/ui.h"
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class PanelScrollPane : public Pane, public LayoutManager
{
public:
	PanelScrollPane( Component& aComponent, unsigned int width = 2000, unsigned int height = 2560, bool buffered = false );
	virtual ~PanelScrollPane();

	//	public virtual methods (EvenListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)	
	virtual void doLayout( const GraphicsContext& gc );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public methods
	void setScrollerBackground( const openocl::imaging::Color& aColor );
	void setComponent( Component& aComponent );
	void setConstrainWidth( bool constrain );
	void setConstrainHeight( bool constrain );
	void resetOffsets();
	void setOffsets( unsigned int x, unsigned int y );

	//	public virtual constant methods (LayoutManager)
	virtual void doLayout( Container& aContainer, const GraphicsContext& gc ) const;
	virtual Dimensions preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const;
	//virtual unsigned int preferredLayoutWidth( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;
	//virtual unsigned int preferredLayoutHeight( const openocl::util::Sequence& components, const GraphicsContext& gc ) const;

	//	public virtual constant methods (Component)
	//Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

private:
#ifdef OPENOCL_UI_SCROLLPANE_USE_BUFFERED_SCROLLER
	Scroller* scroller;
#elif  OPENOCL_UI_SCROLLPANE_USE_DRAWING_SCROLLER
	Scroller* scroller;
#else
	PanelScroller* scroller;
#endif
	ScrollBar* horizontal;

	ScrollBar* vertical;

	Region* pane;
	Region* viewable;
	Region* vbar;
	Region* hbar;
};

};};

#endif

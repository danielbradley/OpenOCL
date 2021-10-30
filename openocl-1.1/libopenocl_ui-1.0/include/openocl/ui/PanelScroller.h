/*
 *  Copyright (C) 1997-2006 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_PANELSCROLLER_H
#define OPENOCL_UI_PANELSCROLLER_H

#include "openocl/ui/Scroller.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/imaging/Color.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class PanelScroller : public Component, public openocl::base::event::EventListener
{
public:
	PanelScroller( Component& aComponent );
	virtual ~PanelScroller();

	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void setContainingPanel( Panel& aPanel );
	virtual void resetContainingPanel();
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	//	public methods
	void setBackground( const openocl::imaging::Color& aColor );
	void setComponent( Component& aComponent );
	void setConstrainWidth( bool constrain );
	void setConstrainHeight( bool constrain );
	void setXOffset( unsigned int offset );
	void setYOffset( unsigned int offset );

	//	public virtual const methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	//	public const methods
	const Component& getComponent() const;

	unsigned int getScrollWidth() const;
	unsigned int getScrollHeight() const;
	unsigned int getXOffset() const;
	unsigned int getYOffset() const;

private:
	Component* com;
	Panel* view;
	Panel* content;

	openocl::imaging::Color* background;

	int constrainWidth;
	int constrainHeight;

	int xOffset;
	int yOffset;

	unsigned int scrollWidth;
	unsigned int scrollHeight;

	bool         redrawAll;
};

};};

#endif

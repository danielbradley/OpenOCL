/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SCROLLER_H
#define OPENOCL_UI_SCROLLER_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class Scroller : public Component, public openocl::base::event::EventListener
{
public:
	Scroller( Component& aComponent );
	virtual ~Scroller();

	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void setContainingPanel( Panel& aPanel );
	virtual void doLayout( const GraphicsContext& gc );
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );

	//	public methods
	void setBackground( const openocl::imaging::Color& aColor );
	void setComponent( Component& aComponent );
	void setConstrainWidth( bool constrain );
	void setConstrainHeight( bool constrain );
	void setXOffset( unsigned int offset );
	void setYOffset( unsigned int offset );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;
	virtual bool draw( GraphicsContext& gc ) const;

	const Component& getComponent() const;
	
	unsigned int getScrollWidth() const;
	unsigned int getScrollHeight() const;
	unsigned int getXOffset() const;
	unsigned int getYOffset() const;

private:
	Component* component;
	openocl::imaging::Color* background;

	unsigned int scrollWidth;
	unsigned int scrollHeight;

	unsigned int xOffset;
	unsigned int yOffset;
	
	bool constrainWidth;
	bool constrainHeight;
};

};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CONTROL_H
#define OPENOCL_UI_CONTROL_H

#include "openocl/ui.h"
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base.h>
#include <openocl/base/Values.h>
#include <openocl/base/event.h>
#include <openocl/base/event/EventListener.h>
#include <openocl/ui/Component.h>

namespace openocl {
  namespace ui {

// Abstract
class Control : public Component, public openocl::base::event::EventListener, public openocl::base::Values
{
public:
	Control( const openocl::base::String& name );
	virtual ~Control();
	
	//	public virtual methods (EventListener)
	virtual void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void doLayout( const GraphicsContext& gc );
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void setBounds( int x, int y, unsigned int width, unsigned int height );
	virtual void setContainingPanel( Panel& aPanel );
	virtual void resetContainingPanel();

	//	public virtual abstract methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent ) = 0;

	//	public virtual methods (Control)
	virtual void refresh();

	//	public methods (Control)
	void setControlComponent( Component& aComponent );
	void setControlTitle( const openocl::base::String& aTitle );
	void setResizeable( bool isControlResizeable );
	FocusManager& getFocusManager();

	//	public virtual constant methods (Values)
	bool                         hasKey( const openocl::base::String& name ) const;
	openocl::base::Iterator*     keys() const;
	const openocl::base::String& findValue( const openocl::base::String& componentName, const openocl::base::String& defaultValue ) const;
	const openocl::base::String& findValue( const openocl::base::String& componentName ) const
		throw (openocl::base::NoSuchElementException*);

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;
	virtual bool isResizeable() const;

	//	public constant methods (Control)
	const openocl::base::String& getControlTitle() const;

protected:
	//	protected methods (Control)
	void setDisableForwardedEvents();
	void setDisableLayout();
	void setDisableDraw();

private:
	FocusManager* focusManager;
	Component* controlComponent;
	openocl::base::String* controlTitle;
	bool resizeable;
	bool redrawAll;

	bool disableForwardedEvents;
	bool disableLayout;
	bool disableDraw;
};

};};

#endif

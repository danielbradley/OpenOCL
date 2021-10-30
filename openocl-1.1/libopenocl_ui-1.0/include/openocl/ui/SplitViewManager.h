/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SPLITVIEWMANAGER_H
#define OPENOCL_UI_SPLITVIEWMANAGER_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class SplitViewManager : public Component, public openocl::base::event::EventListener
{
public:
	static unsigned int padding;
	static unsigned int commandHeight;

	SplitViewManager( flags layoutHints );
	virtual ~SplitViewManager();

	//	public virtual methods (EventListener)
	void deliverEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Component)
	virtual void setContainingPanel( Panel& aPanel );
	virtual void doLayout( const GraphicsContext& gc );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public methods
	virtual void add( Component& aComponent );
	virtual void remove( Component& aComponent );

	//	public virtual constant methods (Component)
	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	bool draw( GraphicsContext& gc ) const;

private:
	SplitView* splitView;

	Region* outline;
	Region* command;
	Region* active;
};

};};

#endif

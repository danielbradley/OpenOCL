/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TABBEDVIEW_H
#define OPENOCL_UI_TABBEDVIEW_H

#include "openocl/ui.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/Dimensions.h"
#include <openocl/base.h>
#include <openocl/imaging.h>
#include <openocl/util.h>

namespace openocl {
	namespace ui {

class TabbedView : public Container
{
public:
	static unsigned int tabWidth;
	static unsigned int tabHeight;

	TabbedView();
	virtual ~TabbedView();

	//	public virtual methods (Component)
	virtual void doLayout( const GraphicsContext& gc );
	virtual void forwardEvent( const openocl::base::event::Event& anEvent );
	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Container)
	virtual void add( Component& component );
	virtual void add( Component* component );

	virtual void remove( Component& component );

	//	public virtual const methods (Component)
	Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	bool draw( GraphicsContext& gc ) const;


	//	public methods
	Component& getActiveComponent() throw (openocl::base::NoSuchElementException*);

	//	public constant methods
	const Component& getActiveComponent() const throw (openocl::base::NoSuchElementException*);

private:
	Region* bounds;
	Region* outline;
	Region* closeTab;
	Region* tabarea;
	Region* active;
	
	unsigned int nrOfComponents;
	unsigned int activeId;
};

};};

#endif

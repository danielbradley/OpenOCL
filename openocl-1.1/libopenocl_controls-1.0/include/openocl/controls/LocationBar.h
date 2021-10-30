/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_LOCATIONBAR_H
#define OPENOCL_CONTROLS_LOCATIONBAR_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/ui/Control.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class LocationBar : public openocl::ui::Control
{
public:
	static unsigned int padding;

	LocationBar( const openocl::base::String& name, unsigned int width=0xFFFF );
	virtual ~LocationBar();

	//	public virtual methods (Component)
//	virtual void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public methods
	virtual void setLabel( const openocl::base::String& aLabel );
	virtual void setLocation( const openocl::base::String& location );

	//	public constant methods
	virtual const openocl::base::String& getLocation() const;
	
private:
	//	private methods
	//void checkLocation( const openocl::base::String& aLocation );
	//void guessLocation( const openocl::base::String& aPartialLocation );

	bool fire;

	openocl::ui::Container* base;
};

};};

#endif

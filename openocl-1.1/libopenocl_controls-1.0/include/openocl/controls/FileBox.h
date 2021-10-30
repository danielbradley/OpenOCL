/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_FILEBOX_H
#define OPENOCL_CONTROLS_FILEBOX_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/io.h>
#include <openocl/ui/Control.h>
#include <openocl/ui/Dimensions.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class FileBox : public openocl::ui::Control
{
public:
	static unsigned int padding;

	FileBox( const openocl::base::String& initialLocation );
	virtual ~FileBox();

	//	public virtual methods (Component)
	//virtual void refresh( const openocl::base::String& changedObjectId );
	virtual void doLayout( const openocl::ui::GraphicsContext& gc );

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public methods
	void setLocation( const openocl::base::String& location ); 
	void setShowHidden( bool show );

	//	public constant methods
	const openocl::base::String& getLocation() const; 

private:
	openocl::ui::Container* base;
	
	bool doRefresh;
};

};};

#endif

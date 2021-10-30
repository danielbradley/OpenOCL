/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_CONTROLS_FILECHOOSER_H
#define OPENOCL_CONTROLS_FILECHOOSER_H

#include <openocl/base/event.h>
#include <openocl/controls.h>
#include <openocl/ui/Control.h>
#include <openocl/ui.h>

namespace openocl {
	namespace controls {

class FileChooser : public openocl::ui::Control
{
public:
	FileChooser();
	virtual ~FileChooser();

	//	public virtual methods (Component)
	virtual void setContainingPanel( openocl::ui::Panel& aPanel );

	//	public virtual methods (Control)
	virtual bool handleActionEvent( const openocl::base::event::ActionEvent& anActionEvent );

	//	public methods
	void resetTitle();
	void setFilename( const openocl::base::String& location );
	void setLocation( const openocl::base::String& location );
	void setPrefix( const openocl::base::String& prefix );
	void setWarnIfExists( bool warnIfExists );
	void setDefaultFilterExtension( const openocl::base::String& filterExtension );
	void addFilterExtension( const openocl::base::String& filterExtension );

	//	public constant methods
	const openocl::base::String& getChosenFile() const;
	const openocl::base::String& getFilter() const;
	const openocl::base::String& getLocation() const;

private:
	void reset();

	openocl::ui::Container* base;
	
	openocl::base::String* prefix;
	openocl::base::String* location;
	openocl::base::String* filter;
	
	bool warnIfExists;
};

};};

#endif

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SELECTOR_H
#define OPENOCL_UI_SELECTOR_H

#include "openocl/ui/Textual.h"
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {

class Selector : public openocl::ui::Textual
{
public:
	Selector( const openocl::base::String& name );
	Selector( const openocl::base::String& name, const openocl::base::String& action );
	Selector( const openocl::base::String& name, const openocl::base::String& action, const openocl::base::String& data );
	~Selector();

	//	public virtual methods (Selector)
	void setAction( const openocl::base::String& action );
	void setData( const openocl::base::String& data );
	void setLabel( const openocl::base::String& label );

	//	public virtual constant methods (Textual)
	const openocl::base::String& getAction() const;
	const openocl::base::String& getData() const;
	const openocl::base::String& getLabel() const;

protected:

	//	Fires event of form:
	//
	//	<action>.<class>.name
	//
	//	CONFIRMED.openocl::ui::Button.name

	void fireSelectorEvent( const openocl::base::String& className );

private:
	openocl::base::String* label;
	openocl::base::String* action;
	openocl::base::String* data;
};

};};

#endif

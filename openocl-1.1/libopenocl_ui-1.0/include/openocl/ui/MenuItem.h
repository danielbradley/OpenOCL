/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_MENUITEM_H
#define OPENOCL_UI_MENUITEM_H

#include "openocl/ui/Selector.h"
#include "openocl/ui.h"
#include <openocl/base/event.h>
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class MenuItem : public openocl::ui::Selector
{
public:
	static unsigned int padding;

	MenuItem( const openocl::base::String& name );
	MenuItem( const openocl::base::String& name, const openocl::base::String& action );
	MenuItem( const openocl::base::String& name, const openocl::base::String& action, const openocl::base::String& data );
	~MenuItem();

	//	public virtual methods (Component)
	void doLayout( const GraphicsContext& gc );
	void processEvent( const openocl::base::event::Event& anEvent );

	//	public virtual const methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;


private:
	Region* text;
	openocl::base::String* label;
	bool    pressed;
	bool    valid;

};

};};

#endif

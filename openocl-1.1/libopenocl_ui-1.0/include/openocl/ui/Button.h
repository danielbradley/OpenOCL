/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_BUTTON_H
#define OPENOCL_UI_BUTTON_H

#include "openocl/ui/Selector.h"
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {

class Button : public openocl::ui::Selector
{
public:
	static unsigned int padding;

	Button( const openocl::base::String& name );
	Button( const openocl::base::String& name, const openocl::base::String& action );
	Button( const openocl::base::String& name, const openocl::base::String& action, const openocl::base::String& label );
	~Button();

	void init();

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	virtual unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	virtual unsigned int getPreferredHeight( const GraphicsContext& gc ) const;

private:
	const openocl::imaging::Image* image;

	bool valid;
	unsigned int textWidth;
	unsigned int textHeight;
	
	Region* bounds;
	Region* button;
	Region* text;

	bool pressed;
};

};};

#endif

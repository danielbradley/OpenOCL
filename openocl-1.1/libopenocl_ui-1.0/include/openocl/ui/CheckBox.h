/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CHECKBOX_H
#define OPENOCL_UI_CHECKBOX_H

#include "openocl/ui/Selector.h"
#include <openocl/imaging.h>

namespace openocl {
	namespace ui {

class CheckBox : public openocl::ui::Selector
{
public:
	static unsigned int size;

	CheckBox( const openocl::base::String& name );
	CheckBox( const openocl::base::String& name, const openocl::base::String& action );
	CheckBox( const openocl::base::String& name, const openocl::base::String& action, const openocl::base::String& label );
	~CheckBox();

	//	public virtual methods (Component)
	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	//	public methods
	void setLayoutHints( flags layoutHints );

	//	public constant methods
	const openocl::base::String& getState() const;
	bool isChecked() const;

private:
	void init();

	unsigned int textWidth;
	unsigned int textHeight;
	
	Region* outline;
	Region* margin;
	Region* text;

	bool valid;
	bool pressed;
	bool selected;

	flags hints;
	openocl::base::String* checked;
	openocl::base::String* unchecked;
};

};};

#endif

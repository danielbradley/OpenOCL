/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TEXTFIELD_H
#define OPENOCL_UI_TEXTFIELD_H

#include "openocl/ui/Textual.h"
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class TextField : public Textual
{
public:
	TextField( const openocl::base::String& aString );
	~TextField();

	//virtual void doLayout( const GraphicsContext& gc );

	virtual void processEvent( const openocl::base::event::Event& anEvent );

	void setData( const openocl::base::String& aString );
	void setHint( const openocl::base::String& aString );
	void setPreferredWidth( unsigned int aWidth );

	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	//	public methods
	void setEditable( bool editable );

	//	public virtual constant methods
	const openocl::base::String& getText() const;
	const openocl::base::String& getData() const;
	const openocl::base::String& getHint() const;

	//	public constant methods
	bool isEditable() const;

private:
	openocl::base::StringBuffer* value;
	openocl::base::String* hintValue;
	
	Region* hint;

	bool editable;
	bool active;
	unsigned int preferredWidth;

	int cursorPosition;
	int clickPoint;
};

};};

#endif

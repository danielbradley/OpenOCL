/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_LABEL_H
#define OPENOCL_UI_LABEL_H

#include "openocl/ui/Textual.h"

namespace openocl {
  namespace ui {

class Label : public openocl::ui::Textual
{
public:
	Label( const openocl::base::String& aString );
	~Label();

	//	public methods
	void setText( const openocl::base::String& aLabel );
	void setPreferredWidth( unsigned int aWidth );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

	//	public virtual constant methods (Textual)
	const openocl::base::String& getData() const;

private:
	openocl::base::String* label;
	unsigned int preferredWidth;
};

};};

#endif

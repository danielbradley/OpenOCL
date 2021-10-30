/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TEXTAREA_H
#define OPENOCL_UI_TEXTAREA_H

#include "openocl/ui/Textual.h"
#include <openocl/base.h>

namespace openocl {
  namespace ui {

class TextArea : public openocl::ui::Textual
{
public:
	static unsigned int padding;

	TextArea( const openocl::base::String& aString );
	~TextArea();

	//	public virtual methods (Component)
	virtual void doLayout( const GraphicsContext& gc );

	//	public methods
	void setText( const openocl::base::String& text );

	//	public virtual constant methods (Component)
	virtual bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;

	//	public virtual constant methods (Textual)
	virtual const openocl::base::String& getText() const;
	virtual const openocl::base::String& getData() const;

private:
	void breakLines( const GraphicsContext& gc, unsigned int maxLineWidth );

	openocl::base::String*   text;
	openocl::util::Sequence* lines;
	unsigned int lineSpacing;
};

};};

#endif

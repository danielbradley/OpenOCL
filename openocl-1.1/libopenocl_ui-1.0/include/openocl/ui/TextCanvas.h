/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TEXTCANVAS_H
#define OPENOCL_UI_TEXTCANVAS_H

#include "openocl/ui/Canvas.h"
#include "openocl/ui.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>
#include <openocl/imaging.h>

namespace openocl {
  namespace ui {

class TextCanvas : public openocl::ui::Canvas
{
public:
	static const unsigned int DEFAULT_MARGIN  = 20;
	static const unsigned int DEFAULT_SPACING = 3;

	TextCanvas();
	~TextCanvas();

	//	public methods
	void setMargin( unsigned int size );
	void setSpacing( unsigned int size );
	void setText( const openocl::base::String& text );
	void appendText( const openocl::base::String& text );

	//	public virtual methods (Component)
	virtual Dimensions getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool drawCanvas( GraphicsContext& gc ) const;

	//	public methods
	void setFontInfo( const openocl::imaging::FontInfo& fontInfo );
	void setBrush( const openocl::imaging::Color& aColor );
	void setBackground( const openocl::imaging::Color& aColor );

	//	public constant methods
	unsigned int getMargin() const;
	unsigned int getSpacing() const;

private:
	openocl::base::StringBuffer* textBuffer;

	unsigned int margin;
	unsigned int spacing;
	
	openocl::imaging::FontInfo* fontInfo;
	openocl::base::String* fontId;
	
	openocl::imaging::Color* brush;
	openocl::imaging::Color* background;
};

};};

#endif

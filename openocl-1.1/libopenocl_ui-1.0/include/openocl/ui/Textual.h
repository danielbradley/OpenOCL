/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_TEXTUAL_H
#define OPENOCL_UI_TEXTUAL_H

#include "openocl/ui/Component.h"
#include <openocl/base.h>

namespace openocl {
	namespace ui {

class Textual : public openocl::ui::Component
{
public:
	//	Text alignment
	//
	//	These values also correspond to the values
	//	defined in LayoutManager.
	
	static const mask LEFT      = 0x0100;
	static const mask CENTERED  = 0x0200;
	static const mask RIGHT     = 0x0400;
	static const mask JUSTIFIED = 0x0800;

	static const mask TOP       = 0x1000;
	static const mask MIDDLE    = 0x2000;
	static const mask BOTTOM    = 0x4000;

	static const mask ALIGN     = 0x0F00;
	static const mask VALIGN    = 0xF000;

	//	Default values
	
	static const unsigned int DEFAULT_INDENT   = 0;
	static const unsigned int DEFAULT_MARGIN   = 5;
	static const unsigned int DEFAULT_SPACING  = 3;
	static const unsigned int DEFAULT_TABWIDTH = 8;

	Textual();
	Textual( const openocl::base::String& name );
	~Textual();

	//	public virtual methods
	void doLayout( const GraphicsContext& gc );

	//	public methods
	void setMarginMM( int mm );
	void setIndent( int value );
	void setIndentMM( int value );
	void setRightIndent( int value );
	void setRightIndentMM( int value );
	void setSpacing( unsigned int value );

	void setTabWidth( unsigned int value );
	void setTabWidthMM( unsigned int value );
	
	void setTextLayoutHints( flags layoutHints );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const openocl::ui::GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( openocl::ui::GraphicsContext& gc ) const;

	//	public abstract virtual constant methods
	virtual const openocl::base::String& getData() const = 0;

	//	public virtual constant methods
	virtual Dimensions getPreferredTextDimensions( const openocl::ui::GraphicsContext& gc ) const;

	//	public constant methods
	Region getTextRegion() const;

	int getIndent() const;
	int getIndentMM() const;
	int getRightIndent() const;
	int getRightIndentMM() const;
	int getMarginMM() const;

	unsigned int getTabWidth() const;
	unsigned int getTabWidthMM() const;

	flags getTextLayoutHints() const;

private:
	Region* text;

	int spacing;
	int indent;
	int rightIndent;

	unsigned int tabWidth;
	
	static double pixel2mm;
	static double mm2pixel;

	flags textHints;
};

};};

#endif

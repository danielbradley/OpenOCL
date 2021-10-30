/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_SCROLLBAR_H
#define OPENOCL_UI_SCROLLBAR_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"

namespace openocl {
	namespace ui {

class ScrollBar : public openocl::ui::Component
{
public:
	static const int HORIZONTAL = 1;
	static const int VERTICAL = 2;

	static unsigned int bredth;
	static unsigned int minimumSliderLength;
	static unsigned int padding;

	ScrollBar( int TYPE );
	~ScrollBar();

	virtual void processEvent( const openocl::base::event::Event& anEvent );
	virtual void doLayout( const GraphicsContext& gc );

	void setAttributes( unsigned int paneSize, unsigned int paneViewableSize );
	void setIncrement( unsigned int increment );
	void setLogicalOffset( int newOffset );

	void scrollDecrease();
	void scrollIncrease();

	bool draw( GraphicsContext& gc ) const;
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
//	unsigned int getPreferredWidth( const GraphicsContext& gc ) const;
//	unsigned int getPreferredHeight( const GraphicsContext& gc ) const;

	unsigned int getOffset() const;

	int getIncrement() const;
	int getType() const;

protected:
	void setOffset( int newOffset );

private:
	void scrollDecrease( unsigned int increment );
	void scrollIncrease( unsigned int increment );
	void reconfigure();
	void initArrows( GraphicsContext& gc );

	int type;

	//	Precomputed points
	Region* scrollBar;
	Region* outline;
	Region* sliderTrough;
	Region* slider;
	Region* decreaseButton;
	Region* increaseButton;

	//	Scroll bar attributes
	int	barLength;
	int	sliderBeginOffset;
	int	sliderEndOffset;
	int	sliderLength;

	//	Controller attributes
	int	scrollSize;
	int	viewableSize;

	//	Calculated values
	int	maxOffset;
	double	view2scroll;
	double	slider2scroll;
	double	scroll2slider;
	int	tabSize;	
	int	maxPixelOffset;

	int	offset;
	int	pixelOffset;

	unsigned int increment;

	unsigned int clickX;
	unsigned int clickY;

	bool decPressed;
	bool incPressed;

	OffscreenImage* lessArrow;
	OffscreenImage* moreArrow;
	OffscreenImage* thumbTab;
};

};};

#endif

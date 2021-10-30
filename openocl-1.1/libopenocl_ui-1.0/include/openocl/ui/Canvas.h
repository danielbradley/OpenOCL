/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_UI_CANVAS_H
#define OPENOCL_UI_CANVAS_H

#include "openocl/ui/Component.h"
#include "openocl/ui.h"
#include <openocl/base/event/EventListener.h>
#include <openocl/base/event.h>

namespace openocl {
  namespace ui {

class Canvas : public openocl::ui::Component
{
public:
	static const unsigned int VIEWABLE_WIDTH  = 0x0000;
	static const unsigned int VIEWABLE_HEIGHT = 0x0000;
	static const unsigned int NEEDED_WIDTH    = 0xFFFFFFFF;
	static const unsigned int NEEDED_HEIGHT   = 0xFFFFFFFF;

	Canvas();
	~Canvas();

	//	public methods (Canvas)
	void decreaseZoom( double change );
	void increaseZoom( double change );
	void setCanvasChanged();
	void setCanvasDimensions( Dimensions d );
	void setScale( double scale );
	void setZoom( double zoom );

	//	public virtual constant methods (Component)
	virtual Dimensions getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual Dimensions getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const;
	virtual bool draw( GraphicsContext& gc ) const;

	//	public constant methods (Canvas)
	virtual bool hasCanvasChanged() const;

	//	public abstract constant methods (Canvas)
	virtual bool       drawCanvas( GraphicsContext& gc ) const = 0;

protected:
	//	public constant methods (Canvas)
	Dimensions getCanvasDimensions() const;
	
private:

	Dimensions dimensions;
	double     scale;
	bool       canvasChanged;
};

};};

#endif

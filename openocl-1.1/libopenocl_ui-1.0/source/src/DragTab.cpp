/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/DragTab.h"

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"

#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

DragTab::DragTab() : Component( "drag_tab" )
{
	this->dragTab = null;
}

DragTab::~DragTab()
{}

/*
 *	Components processEvents to determine whether or not
 *	the event affects it. In the case of a button if the
 *	event occurred within the bounds of the button it
 *	should fire off an Action Event.
 */
void
DragTab::processEvent( const Event& anEvent )
{}

Dimensions
DragTab::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	return Dimensions( 30, 30 );
}


bool
DragTab::draw( GraphicsContext& gc ) const
{
	if ( !this->dragTab )
	{
		const_cast<DragTab*>( this )->initDragTab( gc );
	}

	unsigned int x = this->getX();
	unsigned int y = this->getY();
	unsigned int w = this->getWidth();
	unsigned int h = this->getHeight();

	unsigned int tab_w = this->dragTab->getDrawableWidth();
	unsigned int tab_h = this->dragTab->getDrawableHeight();

	unsigned int offset_x = (x + (w/2)) - (tab_w/2); 
	unsigned int offset_y = (y + (h/2)) - (tab_h/2); 

	gc.copyArea( *this->dragTab, 0, 0, 20, 20, offset_x, offset_y );
	gc.setForeground( Color::DARK_GREY );
	gc.drawLine( 0, h - 1, w, h - 1 );
	gc.setForeground( Color::WHITE );
	gc.drawLine( 0, h - 2, w, h - 2 );

	return false;
}

void
DragTab::initDragTab( GraphicsContext& gc )
{
	this->dragTab  = gc.createOffscreenImage( 20, 20 );
	this->dragTab->clear();

	GraphicsContext& tt_gc = this->dragTab->getGraphicsContext();
	
	tt_gc.setForeground( Color::SHADOW_GREY );
	tt_gc.drawLine( 5, 10, 10, 5 );

	tt_gc.setForeground( Color::SHADOW_GREY );
	tt_gc.drawLine( 5, 14, 14, 5 );

	tt_gc.setForeground( Color::SHADOW_GREY );
	tt_gc.drawLine( 9, 14, 14, 9 );
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ArrangedLayout.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/OffscreenImageManager.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SidewaysLayout.h"
#include "openocl/ui/ToolBar.h"
#include "openocl/ui/ToolBarGroup.h"
#include "openocl/ui/Visual.h"
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/imaging/Color.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int ToolBarGroup::padding = 5;

ToolBarGroup::ToolBarGroup() : OpaqueContainer( Color::TOOLBAR )
{
	this->logoId = null;

	this->setName( "tool_bar_group" );

	this->toolbars = new Container();
	this->toolbars->setName( "toolbars" );
	this->toolbars->setPadding( 0 );
	this->toolbars->setLayout( new SidewaysLayout() );

	this->add( *this->toolbars );

	this->border = new Region();
	this->bars   = new Region();
	this->logo   = new Region();

}

ToolBarGroup::~ToolBarGroup()
{
	delete this->border;
	delete this->bars;
	delete this->logo;
	
	this->remove( *this->toolbars );
	
	delete toolbars;
	delete this->logoId;
}

void
ToolBarGroup::setLogoId( const String& anImageId )
{
	this->logoId = new String( anImageId );
}

void
ToolBarGroup::addToolBar( ToolBar& aToolBar )
{
	this->toolbars->add( aToolBar );
}

void
ToolBarGroup::removeToolBar( ToolBar& aToolBar )
{
	this->toolbars->remove( aToolBar );
}

void
ToolBarGroup::doLayout( const GraphicsContext& gc )
{
	unsigned int padding = this->padding;

	Region bounds = this->getBounds();
	Region border;
	Region bars;
	Region logo;

	int space = (padding*2) + 2;

	try
	{
		if ( this->logoId )
		{
			const OffscreenImage& img = gc.getVisual().getOffscreenImageManager().findImage( gc, *this->logoId );
			logo.width  = img.getDrawableWidth() + space + 2;
			logo.height = img.getDrawableHeight() + space + 2;
			logo.x2 = bounds.x2 - padding;
			logo.x1 = logo.x2 - logo.width + 1;
			logo.y1 = bounds.y1 + padding;
			logo.y2 = logo.y1 + logo.height - 1;
		} else {
			throw new NoSuchElementException();
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		
		logo.width = 1;
		logo.height = 1;
		logo.x2 = bounds.x2 - padding;
		logo.x1 = logo.x2 - logo.width + 1;
		logo.y1 = bounds.y1 + padding;
		logo.y2 = logo.y1 + logo.height - 1;
	}

	border = bounds.shrunkBy( padding );

	Dimensions d = this->toolbars->getPreferredDimensions( gc, border.width - 2, border.height - 2 );

	bars.x1 = border.x1 + 1;
	bars.y1 = border.y1 + 1;
	bars.width  = d.width;
	bars.height = d.height;
	bars.x2 = bars.x1 + bars.width - 1;
	bars.y2 = bars.y1 + bars.height - 1;
	
	border.width = bars.width +2;
	border.height = bars.height + 2;
	border.x2 = border.x1 + border.width - 1;
	border.y2 = border.y1 + border.height - 1;

	this->toolbars->setBounds( bars.x1, bars.y1, bars.width, bars.height );
	this->toolbars->doLayout( gc );

	*this->border = border;
	*this->bars   = bars;
	*this->logo   = logo;

	this->Component::setBoundsChanged( false );
}

Dimensions
ToolBarGroup::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions preferred;
	Dimensions logo;
	Dimensions toolbars;
	
	int space = (padding*2) + 2;

	width  -= space;
	height -= space;

	if ( this->logoId )
	{
		try
		{
			const OffscreenImage& img = gc.getVisual().getOffscreenImageManager().findImage( gc, *this->logoId );
			logo.width  = img.getDrawableWidth() + space;
			logo.height = img.getDrawableHeight() + space;

			width  -= logo.width;
			height -= logo.height;
			
		} catch ( NoSuchElementException* ex ) {
			delete ex;
		}
	}

	unsigned int count = this->toolbars->getComponents().getLength();
	if ( 0 < count )
	{
		toolbars = this->toolbars->getPreferredDimensions( gc, width, height );
	}
	
	if ( this->logoId || ( 0 < count) )
	{
		preferred.width += (padding + 2);	//	Padding between toolbars and logo
		preferred.width += (logo.width + toolbars.width);
		preferred.width += space;
		preferred.height = Math::max( logo.height, toolbars.height );
		preferred.height += space;
	}

#ifdef DEBUG_OPENOCL_UI_TOOLBARGROUP
	fprintf( stderr, "ToolBarGroup::getPreferredDimensions(): %u:%u\n", preferred.width, preferred.height );
#endif

	return preferred;
}

bool
ToolBarGroup::draw( GraphicsContext& gc ) const
{
	bool redraw_all = this->isRedrawAll();
	bool draw_again = this->OpaqueContainer::draw( gc );

	if ( redraw_all )//&& changed )
	{
		Region bounds = this->getBounds();
		Region border = *this->border;
		Region logo   = *this->logo;

		if ( bounds.isValid() )
		{
			gc.setForeground( Color::TOOLBAR );
			gc.setFill( Color::TOOLBAR );

			unsigned int count = this->toolbars->getComponents().getLength();

			//
			//	Bounds 3D
			//

			this->drawOutward3DBox( gc, bounds, Color::SHINE1, Color::SHADE2 );

			//
			//	Border 3D
			//

			if ( 0 < count )
			{
				this->drawInward3DBox( gc, border, Color::SHINE2, Color::SHADE1 );
			}
		
			//
			//	Draw Logo
			//

			if ( this->logoId )
			{
				try
				{
					const OffscreenImage& img = gc.getVisual().getOffscreenImageManager().findImage( gc, *this->logoId );
					unsigned int w = img.getDrawableWidth();
					unsigned int h = img.getDrawableHeight();

					logo.x1 = logo.x2 - 2 - w - (padding*2);

					int px = logo.x1 + 1 + padding;
					int py = logo.y1 + 1 + padding;

					gc.copyArea( img, 0, 0, w, h, px, py );

					//this->drawInward3DBox( gc, logo, Color::SHINE1, Color::SHADE1 );

				} catch ( NoSuchElementException* ex ) {
					delete ex;
				}
			}

			//gc.setClip( border.x1 + 1, border.y1 + 1, border.width - 2, border.height - 2 );
			//draw_again = this->toolbars->draw( gc );
			//gc.popClip();
		}
	}
	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}

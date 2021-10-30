/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Icon.h"
#include "openocl/ui/IconGroup.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SidewaysLayout.h"
#include "openocl/ui/ToolBar.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/imaging/Color.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int ToolBar::padding = 4;

ToolBar::ToolBar()
{
	this->icongroup = new IconGroup( IconGroup::ARRANGED );
	this->icons = new Region();

	this->Container::add( *this->icongroup );
}

ToolBar::ToolBar( const String& name ) : Container( name )
{
	this->icongroup = new IconGroup( IconGroup::ARRANGED );
	this->icons = new Region();

	this->Container::add( *this->icongroup );
}

ToolBar::~ToolBar()
{
	this->remove( *this->icongroup );

	delete this->icongroup;
	delete this->icons;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

void
ToolBar::doLayout( const GraphicsContext& gc )
{
	unsigned int padding = this->padding;
	//unsigned int w = this->getWidth();
	//unsigned int h = this->getHeight();
	
	//Dimensions icongroup_dim = this->icongroup->getPreferredDimensions( gc, w - padding*2, h - padding*2 );
	Region bounds = this->getBounds();
	Region icons;

	icons.x1 = bounds.x1 + padding;
	icons.x2 = bounds.x2 - padding;
	icons.y1 = bounds.y1 + padding;
	icons.y2 = bounds.y2 - padding;
	icons.width = icons.x2 - icons.x1 + 1;
	icons.height = icons.y2 - icons.y1 + 1;

//	icons.x1 = bounds.x1 + padding;
//	icons.width = icongroup_dim.width;
//	icons.x2 = icons.x1 + icons.width - 1;
//	icons.y1 = bounds.y1 + padding;
//	icons.height = icongroup_dim.height;
//	icons.y2 = icons.y1 + icons.height - 1;

	this->icongroup->setBounds( icons.x1, icons.y1, icons.width, icons.height );
	this->icongroup->doLayout( gc );
	
	*this->icons = icons;
}

void
ToolBar::add( openocl::ui::Icon& anIcon )
{
	this->icongroup->add( anIcon );
}

void
ToolBar::add( openocl::ui::Icon* anIcon )
{
	this->icongroup->add( anIcon );
}

void
ToolBar::addIcon( const String& name, const String& actionId, const String& aLabel, const String& imageId, unsigned int width, unsigned int height )
{

	Icon* icon = new Icon( name, actionId, aLabel );

	icon->setImageId( imageId );
	//icon->setLabel( aLabel );
	icon->setMaxPreferredWidth( width );
	
	this->add( icon );
}


bool
ToolBar::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	gc.setBackground( Color::TOOLBAR );

	if ( this->hasChanged() )
	{
		if ( this->isRedrawAll() )
		{
			Region bounds = this->getBounds();
			Region icons  = *this->icons;
	
			gc.setFill( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( bounds );
			this->drawOutward3DBox( gc, bounds, Color::SHINE2, Color::SHADE1 );
			
			this->setRedrawAll( false );
		}
		this->Container::draw( gc );
		this->setChanged( false );
	}
	return draw_again;
}

/*
unsigned int
ToolBar::getPreferredWidth( const GraphicsContext& gc ) const
{
	abort();
	unsigned int preferred_width = 0xFFFF;
	preferred_width = this->icongroup->getPreferredWidth( gc );
	preferred_width += (this->padding * 2 );
	return preferred_width;
}

unsigned int
ToolBar::getPreferredHeight( const GraphicsContext& gc ) const
{
	abort();
	unsigned int preferred_height = 0xFFFF;
	preferred_height = this->icongroup->getPreferredHeight( gc );
	preferred_height += (this->padding * 2 );
	return preferred_height;
}
*/
Dimensions
ToolBar::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim = this->icongroup->getPreferredDimensions( gc, width, height );
	dim.width  += this->padding * 2;
	dim.height += this->padding * 2;
	return dim;
}

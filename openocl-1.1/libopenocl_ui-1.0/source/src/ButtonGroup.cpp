/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ButtonGroup.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/imaging/Color.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int ButtonGroup::padding = 10;
unsigned int ButtonGroup::margin = 5;

ButtonGroup::ButtonGroup( flags layoutHints )// : OpaqueContainer( Color::DIALOG )
{
	this->hints = layoutHints;
	this->group   = new Region();
	this->outline = new Region();
	this->buttons = new Region();
	this->width = 0xFFFF;
	this->height = 0xFFFF;
}

ButtonGroup::~ButtonGroup()
{
	delete this->group;
	delete this->outline;
	delete this->buttons;
}

void
ButtonGroup::doLayout( const GraphicsContext& gc )
{
	this->doLayout( *this, gc );
}

void
ButtonGroup::setSize( unsigned int width, unsigned int height )
{
	this->width = width;
	this->height = height;
}

//-----------------------------------------------------------------------------
//	public virtual constant methods (LayoutManager)
//-----------------------------------------------------------------------------

void
ButtonGroup::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	unsigned int padding = this->padding;

	Region bounds = this->getBounds();
	Region group;
	Region outline;
	Region buttons;

	Dimensions d = this->getPreferredDimensions( gc, bounds.width, bounds.height );
	int cx = bounds.x1 + (bounds.width/2);
	int cy = bounds.y1 + (bounds.height/2);

	switch ( LayoutManager::ALIGN & this->hints )
	{
	case LayoutManager::LEFT:
		group.width = Math::min( d.width, bounds.width );
		group.x1 = bounds.x1;
		group.x2 = group.x1 + group.width - 1;
		break;
	case LayoutManager::CENTERED:
		group.width = Math::min( d.width, bounds.width );
		group.x1 = cx - (group.width/2);
		group.x2 = group.x1 + group.width - 1;
		break;
	case LayoutManager::RIGHT:
	default:
		group.width = Math::min( d.width, bounds.width );
		group.x2 = bounds.x2;
		group.x1 = group.x2 - group.width + 1;
		break;
	}

	switch ( LayoutManager::VALIGN & this->hints )
	{
	case LayoutManager::TOP:
		group.height = Math::min( d.height, bounds.height );
		group.y1 = bounds.y1;
		group.y2 = group.y1 + group.height - 1;
		break;
	case LayoutManager::MIDDLE:
		group.height = Math::min( d.height, bounds.height );
		group.y1 = cy - (group.height/2);
		group.y2 = group.y1 + group.height - 1;
		break;
	case LayoutManager::BOTTOM:
	default:
		group.height = Math::min( d.height, bounds.height );
		group.y2 = bounds.y2;
		group.y1 = group.y2 - group.height + 1;
		break;
	}

	outline.x1 = group.x1 + padding;
	outline.x2 = group.x2 - padding;
	outline.width = outline.x2 - outline.x1 + 1;
	outline.y1 = group.y1 + padding;
	outline.y2 = group.y2 - padding;
	outline.height = outline.y2 - outline.y1 + 1;

	buttons.x1 = outline.x1 + margin;
	buttons.x2 = outline.x2 - margin;
	buttons.width = buttons.x2 - buttons.x1 + 1;
	buttons.y1 = outline.y1 + margin;
	buttons.y2 = outline.y2 - margin;
	buttons.height = buttons.y2 - buttons.y1 + 1;

	const Sequence& components = aContainer.getComponents();
	unsigned int nr = components.getLength();

	if ( 0 < nr )
	{
		Iterator* it = aContainer.getComponents().iterator();

		switch ( LayoutManager::DIRECTION & this->hints )
		{
		case NORTHWARD:
		case SOUTHWARD:
			{
				int y = buttons.y1;
				unsigned int comp_height = (buttons.height - ((nr - 1) * margin)) / nr;
				while ( it->hasNext() )
				{
					Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
					com.setBounds( buttons.x1, y, buttons.width, comp_height );
					com.doLayout( gc );
					y += comp_height + margin;
				}
			}
			break;
		case WESTWARD:
		case EASTWARD:
		default:
			{
				int x = buttons.x1;
				unsigned int comp_width = (buttons.width - ((nr - 1) * margin)) / nr;
				while ( it->hasNext() )
				{
					Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
					com.setBounds( x, buttons.y1, comp_width, buttons.height );
					com.doLayout( gc );
					x += comp_width + margin;
				}
			}
		}
		delete it;
	}
	*this->group   = group;
	*this->outline = outline;
	*this->buttons = buttons;
}

Dimensions
ButtonGroup::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	Dimensions dim;
	int padding = this->padding;
	int margin  = this->margin;
	
	const Sequence& components = aContainer.getComponents();
	
	switch ( LayoutManager::DIRECTION & this->hints )
	{
	case LayoutManager::WESTWARD:
	case LayoutManager::EASTWARD:
		{
			unsigned int unit = LayoutManager::maxWidth( components, gc );
			unsigned int nr = components.getLength();
			dim.width  = (nr * unit) + (nr * margin) + margin + (2 * padding);
			dim.height = LayoutManager::maxHeight( components, gc ) + (2 * margin) + (2 * padding);
		}
		break;
	case LayoutManager::NORTHWARD:
	case LayoutManager::SOUTHWARD:
		{
			unsigned int unit = LayoutManager::maxHeight( components, gc );
			unsigned int nr = components.getLength();
			dim.height  = (nr * unit) + (nr * margin) + margin + (2*padding);
			dim.width = LayoutManager::maxWidth( components, gc ) + (2 * margin) + (2 * padding);
		}
		break;
	}
	
	dim.width  = Math::min( dim.width, width );
	dim.height = Math::min( dim.height, height );

	return dim;
}

//-----------------------------------------------------------------------------
//	public virtual constant methods (Component)
//-----------------------------------------------------------------------------

Dimensions
ButtonGroup::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim = this->preferredLayoutDimensions( gc, *this, width, height );
	return dim;
}

bool
ButtonGroup::draw( GraphicsContext& gc ) const
{
	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();
	bool draw_again = this->Container::draw( gc );

	if ( redraw_all && changed )
	{
		Region buttons = *this->outline;
		Region xbuttons = buttons;

		xbuttons.x1--;
		xbuttons.x2++;
		xbuttons.width += 2;
		xbuttons.y1--;
		xbuttons.y2++;
		xbuttons.height += 2;
	
		//this->drawOutward3DBox( gc, xbuttons, Color::SHINE1, Color::SHADE1 );
		//gc.setForeground( Color::SHADOW );
		//gc.drawRectangle( outline.x1 - 1, outline.y1 - 1, outline.width + 2, outline.height + 2 );
		//gc.setForeground( Color::SHINY );
		//gc.drawLine( outline.x2 + 1, outline.y2 + 1, outline.x1 - 1, outline.y2 + 1 );
		//gc.drawLine( outline.x2 + 1, outline.y2 + 1, outline.x2 + 1, outline.y1 - 1 );
#ifdef DEBUG_OPENOCL_UI_BUTTONGROUP
		gc.setForeground( Color::RED );
		gc.drawRectangleRegion( *this->group );
		gc.setForeground( Color::GREEN );
		gc.drawRectangleRegion( *this->outline );
		gc.setForeground( Color::BLUE );
		gc.drawRectangleRegion( *this->buttons );
#endif
	}
	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}


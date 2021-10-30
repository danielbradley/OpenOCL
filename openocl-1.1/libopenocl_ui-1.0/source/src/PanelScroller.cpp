/*
 *  Copyright (C) 1997-2006 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/PanelScroller.h"

#include "openocl/ui/Container.h"
#include "openocl/ui/DirectedLayout.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/LayoutManager.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/OpaqueContainer.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

PanelScroller::PanelScroller( Component& aComponent )
:
//Scroller( aComponent ),
com( &aComponent )
{
	this->redrawAll = true;

	this->view    = null;
	this->content = null;

	this->xOffset = 0;
	this->yOffset = 0;

	this->constrainWidth = false;
	this->constrainHeight = false;

	this->background = null;
}

PanelScroller::~PanelScroller()
{
	if ( this->view )
	{
		if ( this->com )
		{
			this->content->remove( *this->com );
		}
		this->view->removeAll(); //removeEventListener( *this );
		this->content->removeAll(); //removeEventListener( *this->view );
		delete this->content;
		delete this->view;
	}
}

//-------------------------------------------------------------------
//	public virtual methods (EventListener)
//-------------------------------------------------------------------

void
PanelScroller::deliverEvent( const Event& anEvent )
{
	//fprintf( stderr, "PanelScroller::deliverEvent()\n" );

	unsigned int type = anEvent.getType();
	if ( Event::CHANGE_EVENT == type )
	{
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		if ( ChangeEvent::RELAYOUT == ce.getValue() )
		{
			//	See rationale in Container::deliverEvent

			//fprintf( stderr, "\tPanelScroller::deliverEvent: RELAYOUT\n" );
			this->setBoundsChanged( true );
			if ( this->hasContainingPanel() ) this->getContainingPanel().relayout();
		} else {
			//fprintf( stderr, "\tPanelScroller::deliverEvent: CHANGE\n" );
		}
		this->setChanged( true );
		this->fireEvent( anEvent );
	}
	else if ( Event::ACTION_EVENT == type )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		if ( ae.getCommand().startsWith( "REFRESH.openocl::ui::Panel" ) )
		{
			this->fireRefreshEvent( anEvent );
		}
		else
		{
			this->fireEvent( anEvent );
		}
	}
	else
	{
		this->fireEvent( anEvent );
	}
}

//-------------------------------------------------------------------
//	public virtual methods (Component)
//-------------------------------------------------------------------

void
PanelScroller::setContainingPanel( Panel& aPanel )
{
	Component::setContainingPanel( aPanel );
	
	if ( ! this->view )
	{
		this->view    = aPanel.createSubPanel();
		this->view->setName( "view" );
		this->view->setBackground( Color::WHITE );
		this->view->addEventListener( *this );
		this->content = this->view->createSubPanel();
		this->content->setName( "content" );
		this->content->setLayout( new DirectedLayout( LayoutManager::EASTWARD ) );
		this->content->addEventListener( *this->view );
		this->setComponent( *this->com );
		if ( this->background )
		{
			this->content->setBackground( *this->background );
		}
	}
}

void
PanelScroller::resetContainingPanel()
{
	this->Component::resetContainingPanel();
	if ( this->view )
	{
		if ( this->com )
		{
			this->content->remove( *this->com );
		}
		this->view->removeEventListener( *this );
		if ( this->content )
		{
			this->content->removeEventListener( *this->view );
			delete this->content;
		}
		delete this->view;
		this->content = null;
		this->view = null;
	}
}

void
PanelScroller::doLayout( const GraphicsContext& gc )
{
	Region bounds = this->getBounds();
	if ( this->view )
	{
		this->view->setBounds( bounds.x1, bounds.y1, bounds.width, bounds.height );

		Dimensions d;
		if ( this->constrainWidth )
		{
			d = this->content->getPreferredDimensions( gc, bounds.width, 0xFFFF );
		}
		else if ( this->constrainHeight )
		{
			d = this->content->getPreferredDimensions( gc, 0xFFFF, bounds.height );
		}
		else
		{
			d = this->content->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
		}
		this->content->setBounds( -this->xOffset, -this->yOffset, d.width, d.height );
		this->scrollWidth  = d.width;
		this->scrollHeight = d.height;
	} else {
		this->scrollWidth  = bounds.width;
		this->scrollHeight = bounds.height;
	}
}

void
PanelScroller::forwardEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	
	if ( Event::POINTER_EVENT == (Event::POINTER_EVENT & type ) )
	{
		Region bounds = this->getBounds();
		const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
		int x = bounds.x1;
		int y = bounds.y1;
				
		int pe_x = pe.getX();
		int pe_y = pe.getY();

		if ( this->com && bounds.contains( pe_x, pe_y ) )
		{
			int new_x = (pe_x - x) + xOffset;
			int new_y = (pe_y - y) + yOffset;

			PointerEvent* pe2 = dynamic_cast<PointerEvent*>( pe.copy() );
			pe2->setCoordinates( new_x, new_y );
			this->com->forwardEvent( *pe2 );
			delete pe2;
		}
	} else {


		if ( Event::CHANGE_EVENT != (Event::CHANGE_EVENT & type ))
		{
			//	ChangeEvents aren't forwarded as they do not
			//	apply over a panel boundary.

			if ( this->view ) this->view->forwardEvent( anEvent );
		} else {
			this->setChanged( true );
		}
	}
}

void
PanelScroller::setBackground( const Color& aColor )
{
	Color* color = new Color( aColor );
	delete this->background;
	this->background = color;
}

void
PanelScroller::setComponent( Component& aComponent )
{
	if ( this->com && this->content )
	{
		this->content->remove( *this->com );
	}
	this->com = &aComponent;

	if ( this->content ) this->content->add( *this->com );

	this->xOffset = 0;
	this->yOffset = 0;
}

bool
PanelScroller::draw( GraphicsContext& gc ) const
{
	//	As a PanelScroller is a proxy for a Panel
	//	it doesn't need to be redrawn. Panel's redraw
	//	themselves when they're good and ready!

	Region bounds = this->getBounds();
	//Color& bg = this->getBackgroundColor();
	gc.setFill( Color::RED );
	gc.setFill( Color::RED );
	gc.drawFilledRectangleRegion( bounds );

	this->setChanged( false );
	return false;
}

Dimensions
PanelScroller::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	fprintf( stderr, "PanelScroller::getPreferredDimensions(): does anybody call this?\n" );

	Dimensions d;
	d.width  = width;
	d.height = height;
	return d;
}

void
PanelScroller::setXOffset( unsigned int offset )
{
	this->xOffset = offset;
}

void
PanelScroller::setYOffset( unsigned int offset )
{
	this->yOffset = offset;
}

void
PanelScroller::setConstrainWidth( bool constrain )
{
	this->constrainWidth = constrain;
}

void
PanelScroller::setConstrainHeight( bool constrain )
{
	this->constrainHeight = constrain;
}

const Component&
PanelScroller::getComponent() const
{
	return *this->com;
}

unsigned int
PanelScroller::getScrollWidth() const
{
	return this->scrollWidth;
}

unsigned int
PanelScroller::getScrollHeight() const
{
	return this->scrollHeight;
}

unsigned int
PanelScroller::getXOffset() const
{
	return this->xOffset;
}

unsigned int
PanelScroller::getYOffset() const
{
	return this->yOffset;
}

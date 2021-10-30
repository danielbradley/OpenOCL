/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/PanelScrollPane.h"

#include "openocl/ui/BufferedScroller.h"
#include "openocl/ui/Container.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/PanelScroller.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/ScrollBar.h"
#include "openocl/ui/Scroller.h"
#include <openocl/base/String.h>
#include <openocl/base/NoSuchElementException.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
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

PanelScrollPane::PanelScrollPane( Component& com, unsigned int width, unsigned int height, bool buffered )
: Pane( width, height )
{
	this->setEdge( 5 );

	this->horizontal = new ScrollBar( ScrollBar::HORIZONTAL );
	this->vertical   = new ScrollBar( ScrollBar::VERTICAL );

	this->setLayout( *this );

#ifdef OPENOCL_UI_SCROLLPANE_USE_BUFFERED_SCROLLER
		this->scroller = new BufferedScroller( com, 800, 600 );
#elif  OPENOCL_UI_SCROLLPANE_USE_DRAWING_SCROLLER
		this->scroller = new Scroller( com );
#else
		this->scroller = new PanelScroller( com ); // Instantiated later.
#endif
	
	this->add( *this->scroller );
	this->add( *this->vertical );
	this->add( *this->horizontal );

	this->pane     = new Region();
	this->viewable = new Region();
	this->vbar     = new Region();
	this->hbar     = new Region();
}

PanelScrollPane::~PanelScrollPane()
{
	this->remove( *this->scroller );
	this->remove( *this->vertical );
	this->remove( *this->horizontal );

	delete this->horizontal;
	delete this->vertical;
	delete this->scroller;

	delete this->pane;
	delete this->viewable;
	delete this->vbar;
	delete this->hbar;
}

void
PanelScrollPane::deliverEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	if ( Event::CHANGE_EVENT == type )
	{
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		switch ( ce.getValue() )
		{
		case ChangeEvent::RELAYOUT:
			//	When the scroller passes a RELAYOUT ChangeEvent it means that
			//	the size of the scroller may have changed. Therefore we need
			//	to forward a change event to the scroll bars and set our
			//	bounds as changed for relayout.

			this->forwardChangeEvent();
			this->setBoundsChanged( true );
			this->getContainingPanel().relayout();
#ifdef DEBUG_OPENOCL_UI_SCROLLPANE_CHANGEEVENT
			fprintf( stderr, "PanelScrollPane::deliverEvent: REDRAW_RELAYOUT\n" );
			break;
		case ChangeEvent::REDRAW_CHANGED:
			fprintf( stderr, "PanelScrollPane::deliverEvent: REDRAW_CHANGED\n" );
			break;
		case ChangeEvent::REDRAW_PARTIAL:
			fprintf( stderr, "PanelScrollPane::deliverEvent: REDRAW_PARTIAL\n" );
			break;
		case ChangeEvent::REDRAW_ALL:
			fprintf( stderr, "PanelScrollPane::deliverEvent: REDRAW_ALL\n" );
			break;
#endif
		}
		//this->setRedrawAll( true );
		//this->forwardRedrawAllChangeEvent();
		//this->scroller->forwardChangeEvent();
		//this->scroller->forwardRedrawAllChangeEvent();
		this->Container::deliverEvent( anEvent );
	} else {
		this->Container::deliverEvent( anEvent );
	}
}

void
PanelScrollPane::processEvent( const Event& anEvent )
{
	flags type = anEvent.getType();
	if ( Event::MOUSE_EVENT == type )
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
		switch ( me.getButton() )
		{
			case MouseEvent::BUTTON4_PRESS:
				//fprintf( stderr, "ScrollBar::processEvent: Button 4 - Scroll Up\n" );
				this->vertical->scrollDecrease();
				break;
			case MouseEvent::BUTTON5_PRESS:
				//fprintf( stderr, "ScrollBar::processEvent: Button 5 - Scroll Down\n" );
				this->vertical->scrollIncrease();
		}
	}
}				

void
PanelScrollPane::setScrollerBackground( const Color& aColor )
{
	this->scroller->setBackground( aColor );
}

void
PanelScrollPane::setComponent( Component& aComponent )
{
	Panel* containing_panel = null;
	try
	{
		containing_panel = &this->getContainingPanel();
		aComponent.setContainingPanel( *containing_panel );
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	this->scroller->setComponent( aComponent );
	this->resetOffsets();
}

void
PanelScrollPane::setConstrainWidth( bool constrain )
{
	this->scroller->setConstrainWidth( constrain );
}

void
PanelScrollPane::setConstrainHeight( bool constrain )
{
	this->scroller->setConstrainHeight( constrain );
}

void
PanelScrollPane::resetOffsets()
{
	this->scroller->setXOffset( 0 );
	this->scroller->setYOffset( 0 );
	this->horizontal->setAttributes( 1, 1 );
	this->vertical->setAttributes( 1, 1 );
}

void
PanelScrollPane::setOffsets( unsigned int x, unsigned int y )
{
	this->horizontal->setLogicalOffset( x );
	this->vertical->setLogicalOffset( y );

	this->forwardChangeEvent();
	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}

void
PanelScrollPane::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Dimensions hdim = this->horizontal->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );
	Dimensions vdim = this->vertical->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

	Region bounds  = this->getBounds();
	Region edge    = this->getEdgeRegion();
	Region pane;
	Region viewable;
	Region vbar;
	Region hbar;

	//bounds.x1 = this->getX();
	//bounds.y1 = this->getY();
	//bounds.width = this->getWidth();
	//bounds.height = this->getHeight();
	//bounds.x2 = bounds.x1 + bounds.width - 1;
	//bounds.y2 = bounds.y1 + bounds.height - 1;

	vbar.width  = vdim.width;
	hbar.height = hdim.height;

	pane.x1 = edge.x1;
	pane.y1 = edge.y1;
	pane.x2 = edge.x2 - vbar.width - 1;
	pane.y2 = edge.y2 - hbar.height - 1;
	pane.width = pane.x2 - pane.x1 + 1;
	pane.height = pane.y2 - pane.y1 + 1;

	viewable.x1 = pane.x1 + 1;
	viewable.x2 = pane.x2 - 1;
	viewable.y1 = pane.y1 + 1;
	viewable.y2 = pane.y2 - 1;
	viewable.width = viewable.x2 - viewable.x1 + 1;
	viewable.height = viewable.y2 - viewable.y1 + 1;

	vbar.x2 = edge.x1;
	vbar.x1 = edge.x2 - vbar.width + 1;
	vbar.y1 = edge.y1;
	vbar.y2 = pane.y2;
	vbar.height = vbar.y2 - vbar.y1 + 1;
	vbar.width = vbar.width;

	hbar.x1 = pane.x1;
	hbar.x2 = pane.x2;
	hbar.y2 = edge.y2;
	hbar.y1 = edge.y2 - hbar.height + 1;
	hbar.width = hbar.x2 - hbar.x1 + 1;
	hbar.height = hbar.height;

	*this->pane = pane;
	*this->viewable = viewable;
	*this->vbar = vbar;
	*this->hbar = hbar;

	this->vertical->setBounds( vbar.x1, vbar.y1, vbar.width, vbar.height );
	this->vertical->doLayout( gc );
	this->horizontal->setBounds( hbar.x1, hbar.y1, hbar.width, hbar.height );
	this->horizontal->doLayout( gc );


	if ( 0 )
	{
		dynamic_cast<Container*>( this->scroller )->setBounds( viewable.x1, viewable.y1, viewable.width, viewable.height );
		dynamic_cast<Container*>( this->scroller )->doLayout( gc );
	} else {
#ifdef DEBUG_OPENOCL_UI_SCROLLPANE
		fprintf( stderr, "PanelScrollPane::doLayout: %3i:%3i %3i:%3i\n", viewable.x1, viewable.y1, viewable.width, viewable.height );
#endif
		this->scroller->setBounds( viewable.x1, viewable.y1, viewable.width, viewable.height );
		this->scroller->doLayout( gc );
		this->horizontal->setIncrement( this->scroller->getComponent().getLineHeight() );
		this->vertical->setIncrement( this->scroller->getComponent().getLineHeight() );
	}

}

void
PanelScrollPane::doLayout( const GraphicsContext& gc )
{
	/*
	unsigned int x = this->getX();
	unsigned int y = this->getY();
	unsigned int w = this->getWidth();
	unsigned int h = this->getHeight();
	
	unsigned int vsb_width  = this->vertical->getPreferredWidth( gc );
	unsigned int hsb_height = this->horizontal->getPreferredHeight( gc );
	
	unsigned int border_east = (x + w) - vsb_width;
	unsigned int border_south = (y + h) - hsb_height;
	*/

	if ( this->hasChanged() )
	{
		//XXX this->forwardChangeEvent();
	}
	this->Container::doLayout( gc );

	if ( this->scroller )
	{
		this->horizontal->setAttributes( this->scroller->getScrollWidth(), this->scroller->getWidth() );
		this->vertical->setAttributes( this->scroller->getScrollHeight(), this->scroller->getHeight() );
	}
}

Dimensions
PanelScrollPane::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	IO::err().println( "PanelScrollPane::preferredLayoutWidth() should not be called\n" );
	abort();
	Dimensions dim;
	dim.width = 0xFFFF;
	dim.height = 0xFFFF;
	return dim;
}

//Dimensions
//PanelScrollPane::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
//{
//	return Dimensions( width, height );
//}

bool
PanelScrollPane::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;

	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();

	gc.setBackground( Color::TOOLBAR );

	//fprintf( stderr, "PanelScrollPane::draw()\n" );

	Region bounds   = this->getBounds();
	Region edge     = this->getEdgeRegion();
	Region pane     = *this->pane;
	Region viewable = *this->viewable;
	Region hbar     = *this->hbar;
	Region vbar     = *this->vbar;

	if ( redraw_all )
	{
		gc.setForeground( Color::TOOLBAR );
		gc.setFill( Color::TOOLBAR );
		gc.drawFilledRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );
	}
		
	if ( changed )
	{
		this->drawInward3DBox( gc, pane, Color::SHINE1, Color::SHADE2 );

		int offset_x = this->horizontal->getOffset();
		int offset_y = this->vertical->getOffset();
		this->scroller->setXOffset( offset_x );
		this->scroller->setYOffset( offset_y );

		//	Draws the two scroll bars and the scroller pane.
		draw_again = this->Container::draw( gc );

		//const_cast<PanelScrollPane*>( this )->setChanged( false );

		//  gc.setForeground( Color::GREEN );
		//  gc.drawRectangleRegion( outline );
		//  gc.drawRectangleRegion( pane );
		//  gc.drawRectangleRegion( viewable );
		//  gc.drawRectangleRegion( hbar );
		//  gc.drawRectangleRegion( vbar );

	}

	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}

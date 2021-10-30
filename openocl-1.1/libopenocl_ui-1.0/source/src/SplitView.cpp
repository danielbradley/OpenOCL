/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/SplitView.h"

#include "openocl/ui/ColumnLayout.h"
#include "openocl/ui/Cursor.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/Number.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/formats/XPixmap.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

unsigned int SplitView::gapSize = 10;

SplitView::SplitView( flags layoutHints )
{
	this->setEdge( 5 );
	this->setLayout( *this );
	this->hints = layoutHints;
	this->nrOfComponents = 0;

	this->outline = new Region();
	this->gap     = new Region();
	this->fixed   = new Region();
	this->sub     = new Region();
	this->handle  = new Region();

	this->click = false;
	this->selectedCell = 0;
	this->clickX = 0;
	this->clickY = 0;
	this->privilegedWidth = -1;
}

SplitView::~SplitView()
{
	delete this->outline;
	delete this->gap;
	delete this->fixed;
	delete this->sub;
	delete this->handle;
}

void
SplitView::processEvent( const Event& anEvent )
{
	flags type = anEvent.getType();

	bool fire_change = false;
	
	switch ( this->nrOfComponents )
	{
	case 0:
	case 1:
		break;
	default:
		switch ( Event::POINTER_EVENT & type )
		{
		case Event::POINTER_EVENT:
			{
				const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
				int pe_x = pe.getX();
				int pe_y = pe.getY();
				bool local_event = this->contains( pe_x, pe_y );
				bool over_gap = this->gap->contains( pe_x, pe_y );

				int delta = 0;
				//int pe_adj_x = pe_x - adj;

				if ( local_event && over_gap )
				{
					const_cast<Panel&>( this->getContainingPanel() ).setCursor( Cursor::LEFT_RIGHT );
				}				
				if ( !over_gap && !this->click )
				{
					const_cast<Panel&>( this->getContainingPanel() ).resetCursor();
				}

//--------------
				switch ( type )
				{
				case Event::MOUSE_EVENT:
					{
						const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
						if ( me.BUTTON1_PRESS == ( me.BUTTON1_PRESS & me.getButton() ) )
						{
							if ( over_gap )
							{
								this->click = true;
								this->clickX = pe_x;
								this->clickY = pe_y;
								this->clickWidth = this->privilegedWidth;
							}
						}
						else if ( me.BUTTON1_RELEASE == ( me.BUTTON1_RELEASE & me.getButton() ) )
						{
							if ( over_gap )
							{
								//fire_change = true;
							}
							this->click = false;
						}
					}
					break;
				case Event::MOTION_EVENT:
					{
						if ( this->click )
						{
							switch ( this->hints )
							{
							case LayoutManager::WESTWARD:
							case LayoutManager::EASTWARD:
								delta = pe_x - this->clickX;
								break;
							case LayoutManager::NORTHWARD:
							case LayoutManager::SOUTHWARD:
								delta = pe_y -  this->clickY;
								break;
							}
							
							switch ( this->hints )
							{
							case LayoutManager::WESTWARD:
							case LayoutManager::NORTHWARD:
								this->privilegedWidth = this->clickWidth - delta;
								break;
							case LayoutManager::EASTWARD:
							case LayoutManager::SOUTHWARD:
								//	Intentional fallthrough
							default:
				                                this->privilegedWidth = this->clickWidth + delta;
							}
							
							fire_change = true;
						}
					}
				}
//--------------
			}
		}
	}
		//IO::err().printf( "SplitView::ProcessEvent: overSplit( %i ) cell( %u ) click( %i ) widthLeft( %u ) widthRight( %u ) delta( %i )\n",
		//	over_split, cell, this->click, this->widthLeft, this->widthRight, delta );

	if ( fire_change )
	{
		this->fireChangeEvent();
		this->forwardRedrawAllChangeEvent();
		const_cast<Panel&>( this->getContainingPanel() ).relayout();
	}
}

void
SplitView::add( Component* component )
{
	this->Container::add( component );
	this->nrOfComponents++;
	this->nextComponent();
}

void
SplitView::add( Component& component )
{
	this->Container::add( component );
	this->nrOfComponents++;
	this->nextComponent();
}

void
SplitView::nextComponent()
{
	//	Increment through additional components until end then revert to 1.

	this->selectedCell = ++this->selectedCell % this->getComponents().getLength();
	if ( 0 == this->selectedCell )
	{
		this->selectedCell = ++this->selectedCell % this->getComponents().getLength();
	}
}

Dimensions
SplitView::preferredLayoutDimensions( const GraphicsContext& gc, const Container& aContainer, unsigned int width, unsigned int height ) const
{
	unsigned int padding = this->getEdge();
	const Sequence& components = aContainer.getComponents();
	unsigned int nr = components.getLength();

	if ( -1 == this->privilegedWidth && ( 2 <= nr) )
	{
		int privileged_width = -1;
		switch ( this->hints )
		{
		case LayoutManager::SOUTHWARD:
			{
				const Component& com = dynamic_cast<const Component&>( components.elementAt( 0 ) );
				privileged_width = com.getPreferredDimensions( gc, width, height ).height;
			}
			break;
		case LayoutManager::EASTWARD:
			{
				const Component& com = dynamic_cast<const Component&>( components.elementAt( 0 ) );
				privileged_width = com.getPreferredDimensions( gc, width, height ).width;
			}
			break;
		case LayoutManager::NORTHWARD:
			{
				const Component& com = dynamic_cast<const Component&>( components.elementAt( 1 ) );
				privileged_width = com.getPreferredDimensions( gc, width, height ).height;
			}
			break;
		case LayoutManager::WESTWARD:
		default:
			{
				const Component& com = dynamic_cast<const Component&>( components.elementAt( 1 ) );
				privileged_width = com.getPreferredDimensions( gc, width, height ).width;
			}
		}
		const_cast<SplitView*>( this )->privilegedWidth = privileged_width;
	}

	Dimensions dim( 1, 1 );


	switch ( this->hints )
	{
	case LayoutManager::WESTWARD:
	case LayoutManager::EASTWARD:
		dim.width  = sumOfWidths( components, gc );
		dim.width += gapSize * (nr - 1);
		dim.width += padding;
		dim.height = height; // maxHeight( components, gc );
		break;
	case LayoutManager::NORTHWARD:
	case LayoutManager::SOUTHWARD:
		dim.width  = width; //maxWidth( components, gc );
		dim.height = sumOfHeights( components, gc );
		dim.height += gapSize * (nr - 1);
		dim.height += padding;
		break;
	}

	return dim;
}

void
SplitView::doLayout( Container& aContainer, const GraphicsContext& gc ) const
{
	Region bounds = this->getBounds();
	unsigned int padding = this->getEdge();

	int primary_length;

	switch ( this->hints )
	{
	case LayoutManager::WESTWARD:
		primary_length = bounds.width - (2*padding) - this->gapSize - this->privilegedWidth;
		break;
	case LayoutManager::EASTWARD:
		primary_length = this->privilegedWidth;
		break;
	case LayoutManager::NORTHWARD:
		primary_length = bounds.height - (2*padding) - this->gapSize - this->privilegedWidth;
		break;
	case LayoutManager::SOUTHWARD:
		primary_length = this->privilegedWidth;
		break;
	}

	int handle_mid = bounds.y1 + (bounds.height/2);
	Region edge = this->getEdgeRegion();
	Region tgap;
	Region tfixed;
	Region tsub;
	Region thandle;

	const Sequence& components = dynamic_cast<const Sequence&>( aContainer.getComponents() );
	unsigned int nr_of_components = components.getLength();

	{
		switch ( this->hints )
		{
		case LayoutManager::EASTWARD:
		case LayoutManager::WESTWARD:

			tgap.x1 = edge.x1 + primary_length;
			tgap.x2 = tgap.x1 + this->gapSize - 1;
		
			//	Now if fixed panel is outside allowable area move back
			//
			if ( tgap.x1 < (edge.x1 + 1) )
			{
				tgap.x1 = edge.x1 + 1;
				tgap.x2 = tgap.x1 + this->gapSize - 1;
			}
			if ( tgap.x2 > (edge.x2 - 1) )
			{
				tgap.x2 = edge.x2 - 1;
				tgap.x1 = tgap.x2 - this->gapSize + 1;
			}
			
			tgap.y1 = bounds.y1 + padding;
			tgap.y2 = bounds.y1 + bounds.height - padding - 1;
			tgap.width = tgap.x2 - tgap.x1 + 1;
			tgap.height = tgap.y2 - tgap.y1 + 1;
			*this->gap = tgap;

			thandle.x1 = tgap.x1 + (this->gapSize/2);
			thandle.x2 = thandle.x1 + 1;
			thandle.y1 = handle_mid - padding;
			thandle.y2 = handle_mid + padding;
			thandle.width = thandle.x2 - thandle.x1 + 1;
			thandle.height = thandle.y2 - thandle.y1 + 1;
			*this->handle = thandle;

			tfixed.x1 = edge.x1 + 2;
			tfixed.x2 = tgap.x1 - 1;
			tfixed.y1 = tgap.y1 + 2;
			tfixed.y2 = tgap.y2 - 2;
			tfixed.width = tfixed.x2 - tfixed.x1 + 1;
			tfixed.height = tfixed.y2 - tfixed.y1 + 1;
			*this->fixed = tfixed;

			tsub.x1 = tgap.x2 + 1;
			tsub.x2 = edge.x2 - 1;
			tsub.y1 = tfixed.y1 - 1;
			tsub.y2 = tfixed.y2 + 1;
			tsub.width = tsub.x2 - tsub.x1 + 1;
			tsub.height = tsub.y2 - tsub.y1 + 1;
			*this->sub = tsub;
			break;

		case LayoutManager::NORTHWARD:
		case LayoutManager::SOUTHWARD:
			handle_mid = bounds.x1 + (bounds.width/2);

			tgap.y1 = edge.y1 + primary_length;
			tgap.y2 = tgap.y1 + this->gapSize - 1;

			//	Now if fixed panel is outside allowable area move back
			//	
			if ( tgap.y1 < (edge.y1 + 1) )
			{
				tgap.y1 = edge.y1 + 1;
				tgap.y2 = tgap.y1 + this->gapSize - 1;
			}
			if ( tgap.y2 > (edge.y2 - 1) )
			{
				tgap.y2 = edge.y2 - 1;
				tgap.y1 = tgap.y2 - this->gapSize + 1;
			}

			tgap.x1 = bounds.x1 + padding;
			tgap.x2 = bounds.x1 + bounds.width - padding - 1;
			tgap.width = tgap.x2 - tgap.x1 + 1;
			tgap.height = tgap.y2 - tgap.y1 + 1;
			*this->gap = tgap;

			thandle.x1 = handle_mid - padding;
			thandle.x2 = handle_mid + padding;
			thandle.y1 = tgap.y1 + (this->gapSize/2);
			thandle.y2 = thandle.y1 + 1;
			thandle.width = thandle.x2 - thandle.x1 + 1;
			thandle.height = thandle.y2 - thandle.y1 + 1;
			*this->handle = thandle;

			tfixed.x1 = edge.x1 + 2;
			tfixed.x2 = edge.x2 - 2;
			tfixed.y1 = edge.y1 + 1;
			tfixed.y2 = tgap.y1 - 1;
			tfixed.width = tfixed.x2 - tfixed.x1 + 1;
			tfixed.height = tfixed.y2 - tfixed.y1 + 1;
			*this->fixed = tfixed;

			tsub.x1 = tfixed.x1;
			tsub.x2 = tfixed.x2;
			tsub.y1 = tgap.y2 + 1;
			tsub.y2 = edge.y2 - 2;
			tsub.width = tsub.x2 - tsub.x1 + 1;
			tsub.height = tsub.y2 - tsub.y1 + 1;
			*this->sub = tsub;
			break;
		}			
	}

	switch ( nr_of_components )
	{
	case 0:
		break;
	case 1:
		{
			Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 0 ) ) );
			//com.setBounds( tfixed.x1, tfixed.y1, tfixed.width, tfixed.height );
			com.setBounds( edge.x1 + 2, edge.y1 + 2, edge.width - 4, edge.height - 4 );
			com.doLayout( gc );
		}
		break;
	default:
		{
			Component& com = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( 0 ) ) );
			com.setBounds( tfixed.x1, tfixed.y1, tfixed.width, tfixed.height );
			com.doLayout( gc );

			for ( unsigned int i=1; i < nr_of_components; i++ )
			{
				Component& com2 = const_cast<Component&>( dynamic_cast<const Component&>( components.elementAt( i ) ) );
				com2.setBounds( tsub.x1, tsub.y1, tsub.width, tsub.height );
				com2.doLayout( gc );
			}
		}
		break;
	}
}

bool
SplitView::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	bool changed = this->hasChanged();
	bool redraw_all = this->isRedrawAll();

	gc.setBackground( Color::TOOLBAR );
	unsigned int nr_of_components = Math::min( 2, this->getComponents().getLength() );
	unsigned int padding = this->getEdge();

	Region bounds = this->getBounds();
	Region edge   = this->getEdgeRegion();
	Region tgap   = *this->gap;
	Region tsub   = *this->sub;
	Region than   = *this->handle;

	if ( redraw_all )
	{
		gc.setFill( Color::DIALOG );
		gc.drawFilledRectangleRegion( bounds );
	}
	
	if ( changed )
	{
		int smallest = (padding*2) + (this->gapSize*2);
		if ( (bounds.width > smallest) && (bounds.height > smallest) ) 
		{

			//	Draw descent around everything
			this->drawInward3DBox( gc, edge, Color::SHINE2, Color::SHADE1 );
	
			if ( 1 == nr_of_components )
			{
				//	If only one component draw an ascent to make it look like a
				//	panel.

				Region suboutline = edge.shrunkBy( 1 );
		
				this->drawOutward3DBox( gc, suboutline, Color::SHINE2, Color::SHADE1 );
			}
			else if ( 2 == nr_of_components )
			{
				//	Draw handle
				switch ( this->hints )
				{
				case LayoutManager::WESTWARD:
				case LayoutManager::EASTWARD:
					gc.setForeground( Color::SHINY );
					gc.drawLine( than.x2, than.y1, than.x2, than.y2 );
					gc.setForeground( Color::SHADOW );
					gc.drawLine( than.x1, than.y1, than.x1, than.y2 );
					break;
				case LayoutManager::NORTHWARD:
				case LayoutManager::SOUTHWARD:
					gc.setForeground( Color::SHINY );
					gc.drawLine( than.x1, than.y2, than.x2, than.y2 );
					gc.setForeground( Color::SHADOW );
					gc.drawLine( than.x1, than.y1, than.x2, than.y1 );
					break;
				}
			}

			//	Highlight fixed box
			Region suboutline = edge;
			switch ( this->hints )
			{
			case LayoutManager::WESTWARD:
				suboutline.x1 = tgap.x1;
				suboutline.x2 = edge.x2 - 1;
				suboutline.y1 = edge.y1 + 1;
				suboutline.y2 = edge.y2 - 1;
				suboutline.width = suboutline.x2 - suboutline.x1 + 1;
				suboutline.height = suboutline.y2 - suboutline.y1 + 1;
			
				this->drawOutward3DBox( gc, suboutline, Color::SHINE2, Color::SHADE1 );
				break;
			case LayoutManager::EASTWARD:
				suboutline.x1 = edge.x1 + 1;
				suboutline.x2 = tgap.x2;
				suboutline.y1 = edge.y1 + 1;
				suboutline.y2 = edge.y2 - 1;
				suboutline.width = suboutline.x2 - suboutline.x1 + 1;
				suboutline.height = suboutline.y2 - suboutline.y1 + 1;
			
				this->drawOutward3DBox( gc, suboutline, Color::SHINE2, Color::SHADE1 );
				break;
			case LayoutManager::NORTHWARD:
				gc.setForeground( Color::SHADE1 );
				gc.drawLine( edge.x2 - 1, edge.y2 - 1, edge.x2 - 1, tgap.y1 );
				gc.drawLine( edge.x2 - 1, edge.y2 - 1, edge.x1 + 1, edge.y2 - 1 );
			
				gc.setForeground( Color::SHINE2 );
				gc.drawLine( edge.x1 + 1, edge.y2 - 1, edge.x1 + 1, tgap.y1 );
				gc.drawLine( edge.x2 - 1, tgap.y1, edge.x1 + 1, tgap.y1 );
				break;
			case LayoutManager::SOUTHWARD:
				gc.setForeground( Color::SHADE1 );
				gc.drawLine( edge.x2 - 1, edge.y1 + 1, edge.x2 - 1, tgap.y2 );
				gc.drawLine( edge.x1 + 1, tgap.y2, edge.x2 - 1, tgap.y2 );
		
				gc.setForeground( Color::SHINE2 );
				gc.drawLine( edge.x1 + 1, edge.y1 + 1, edge.x1 + 1, tgap.y2 );
				gc.drawLine( edge.x1 + 1, edge.y1 + 1, edge.x2 - 1, edge.y1 + 1 );
				break;
			}
	
			//	Draw 3D impression around sub
			//gc.setForeground( Color::SHADOW );
			//gc.drawLine( tsub.x1, tsub.y1, tsub.x2, tsub.y1 );
			//gc.setForeground( Color::SHINY );
			//gc.drawLine( tsub.x1, tsub.y2, tsub.x2, tsub.y2 );
			//gc.drawLine( tsub.x2, tsub.y1, tsub.x2, tsub.y2 );

			draw_again = this->Container::draw( gc );

			if ( 0 < this->selectedCell )
			{
				const Component& com = dynamic_cast<const Component&>( this->getComponents().elementAt( this->selectedCell ) );
				Region bounds = com.getBounds();
				gc.setClip( bounds.x1, bounds.y1, bounds.width, bounds.height );
				com.draw( gc );
				gc.popClip();
			}
		}
		
	}
	this->setChanged( false );
	this->setRedrawAll( false );

	return draw_again;
}

/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/StepsBox.h"

#include <openocl/base/Environment.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/User.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/Path.h>
#include <openocl/ui/Blank.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/ButtonGroup.h>
#include <openocl/ui/Canvas.h>
#include <openocl/ui/ColumnLayout.h>
#include <openocl/ui/ComboBox.h>
#include <openocl/ui/Control.h>
#include <openocl/ui/Dialog.h>
#include <openocl/ui/DirectedLayout.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/LayoutManager.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/SplitView.h>
#include <openocl/ui/StackedLayout.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/util/Sequence.h>
#include <openocl/util/StringTokenizer.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

namespace openocl {
	namespace controls {
		namespace stepsboxcontrol {

class ListCanvas : public Canvas
{
public:
	ListCanvas( Sequence& stepsControls ) : Canvas()
	{
		this->activeId = -1;
		unsigned int max = stepsControls.getLength();
		for ( unsigned int i=0; i < max; i++ )
		{
			const String& name = dynamic_cast<Component&>( stepsControls.elementAt( i ) ).getName();
			this->items.add( new String( name ) );
		}
		if ( this->items.getLength() ) activeId = 0;
	}
	
	~ListCanvas()
	{}

	Dimensions getPreferredCanvasDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
	{
		if ( 0 == size.width )
		{
			unsigned int max_width = 0;
			unsigned int total_height = 0;
			unsigned int max = this->items.getLength();
			for ( unsigned int i=0; i < max; i++ )
			{
				TextExtents* extents = gc.measureTextExtents( dynamic_cast<const String&>( this->items.elementAt( i ) ) );
				max_width = Math::max( max_width, extents->width );
				total_height += (extents->ascent + extents->descent);
				total_height += 10;
				delete extents;
			}
			const_cast<ListCanvas*>( this )->size.width = 40 + max_width;
			const_cast<ListCanvas*>( this )->size.height = 40 + total_height;
		}
		return this->size;
	}

	bool drawCanvas( GraphicsContext& gc ) const
	{
		Color original_background( gc.getBackground() );
		{
			Region bounds = this->getBounds();
			Region box    = this->getBox();
			
			gc.setFill( Color::DIALOG );
			gc.drawFilledRectangleRegion( bounds );
			gc.setFill( Color::WHITE );
			gc.drawFilledRectangleRegion( box );
		
			int x = bounds.x1 + 20;
			int y = bounds.y1 + 20;

			int max = this->items.getLength();
			for ( int i=0; i < max; i++ )
			{
				const String& str = dynamic_cast<const String&>( this->items.elementAt( i ) );
				TextExtents* extents = gc.measureTextExtents( str );
				//if ( activeId == i )
				//{
				//	gc.setFill( Color::BLUE );
				//	gc.drawFilledRectangle( x - 10, y - 5, bounds.width - 20, extents->ascent + extents->descent + 10 );
				//}

				y += extents->ascent;
				if ( activeId == i )
				{
					gc.setForeground( Color::BLUE );
				} else {
					gc.setForeground( Color::DARK_GREY );
				}
				gc.drawString( str, x, y );
				y += extents->descent;
				y += 10;
				delete extents;
			}
			this->drawInward3DBox( gc, box, Color::SHINE2, Color::SHADE1 );
		}
		gc.setBackground( original_background );
		return false;
	}

	void previous()
	{
		if ( 0 <= (activeId - 1) )
		{
			this->activeId--;
		}
		this->fireChangeEvent();
		this->getContainingPanel().redraw();
	}

	void next()
	{
		if ( (activeId + 1) < ((int) this->items.getLength()) )
		{
			this->activeId++;
		}
		this->fireChangeEvent();
		this->getContainingPanel().redraw();
	}

	int activeId;
	Sequence items;
	Dimensions size;
};

class ContentContainer : public Container
{
public:
	ContentContainer( Sequence& stepsControls )
	: Container(), splitview( LayoutManager::EASTWARD ), list( stepsControls ), steps( stepsControls )
	{
		this->activeId = -1;

		this->splitview.add( this->list );
		if ( this->steps.getLength() )
		{
			this->activeId = 0;
			this->splitview.add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}

		this->add( this->splitview );
	}
	
	~ContentContainer()
	{
		this->splitview.remove( this->list );
		this->splitview.remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		this->remove( this->splitview );
	}

	void previous()
	{
		if ( 0 <= (activeId - 1) )
		{
			this->list.previous();
			this->splitview.remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
			this->activeId--;
			this->splitview.add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}
		this->fireChangeEvent();
		this->getContainingPanel().relayout();
	}

	void next()
	{
		if ( (activeId + 1) < ((int) this->steps.getLength()) )
		{
			this->list.next();
			this->splitview.remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
			this->activeId++;
			this->splitview.add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}
		this->fireChangeEvent();
		this->getContainingPanel().relayout();
	}

	SplitView splitview;
	ListCanvas list;
	Sequence& steps;

	int activeId;
};

class ContentContainer1 : public SplitView
{
public:
	ContentContainer1( Sequence& stepsControls )
	: SplitView( LayoutManager::EASTWARD ), list( stepsControls ), steps( stepsControls )
	{
		this->activeId = -1;
		this->setLayout( new DirectedLayout( LayoutManager::EASTWARD ) );
		this->add( this->list );
		this->add( new Blank() );

		if ( this->steps.getLength() )
		{
			this->activeId = 0;
			//this->add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}
	}
	
	~ContentContainer1()
	{
		this->remove( this->list );
		//this->remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
	}

	void previous()
	{
		if ( 0 <= (activeId - 1) )
		{
			this->list.previous();
			this->remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
			this->activeId--;
			this->add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}
		this->fireChangeEvent();
		this->getContainingPanel().relayout();
	}

	void next()
	{
		if ( (activeId + 1) < ((int) this->steps.getLength()) )
		{
			this->list.next();
			this->remove( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
			this->activeId++;
			this->add( dynamic_cast<Control&>( steps.elementAt( this->activeId ) ) );
		}
		this->fireChangeEvent();
		this->getContainingPanel().relayout();
	}

	ListCanvas list;
	Sequence& steps;

	int activeId;
};

class ButtonsContainer : public ButtonGroup
{
public:
	ButtonsContainer()
	: ButtonGroup( LayoutManager::EASTWARD | LayoutManager::RIGHT | LayoutManager::BOTTOM ),
	  previous( "Previous", "PREVIOUS", "<< Previous" ),
	  next( "Next", "NEXT", "Next >>" ),
	  finish( "Finish", "FINISH", "Finish" )
	{
		this->setLayout( new DirectedLayout( LayoutManager::WESTWARD ) );
		this->add( new Blank() );
		this->add( this->previous );
		this->add( this->next );
		this->add( this->finish );
	}
	
	~ButtonsContainer()
	{
		this->remove( this->previous );
		this->remove( this->next );
		this->remove( this->finish );
	}
	
	Button previous;
	Button next;
	Button finish;
};

class BottomContainer : public OpaqueContainer
{
public:
	BottomContainer()
	: OpaqueContainer( Color::DIALOG )
	{
		this->add( this->buttons );
	}
	
	~BottomContainer()
	{
		this->remove( this->buttons );
	}

	ButtonsContainer buttons;
};

class StepsContainer : public Container
{
public:
	StepsContainer( Sequence& stepsContainers, FocusManager& fm )
	: Container(), steps( stepsContainers ), contents( stepsContainers )
	{
		this->setLayout( new DirectedLayout( LayoutManager::NORTHWARD ) );
		this->setName( "openocl::controls::dialogboxcontrol::StepsContainer" );
		this->add( this->contents );
		this->add( this->bottomButtons );
		
//		fm.addComponent( this->buttons.confirm );
//		fm.addComponent( this->buttons.cancel );
	}
	
	~StepsContainer()
	{
		this->remove( this->contents );
		this->remove( this->bottomButtons );
	}

	virtual void handleActionEvent( const ActionEvent& anActionEvent )
	{
		const String& command = anActionEvent.getCommand();
		
		if ( command.startsWith( "PREVIOUS." ) )
		{
			this->contents.previous();
		}
		else if ( command.startsWith( "NEXT." ) )
		{
			this->contents.next();
		}
			
	}

	virtual void doLayout( const GraphicsContext& gc )
	{
		this->Container::doLayout( gc );
	}

	Sequence& steps;
	ContentContainer contents;
	BottomContainer bottomButtons;
};
	

};};};

//---------------------------------------------------------------------------------
//	public class StepsBox
//---------------------------------------------------------------------------------

StepsBox::StepsBox( Sequence& stepsControls )
: Control( "openocl::controls::StepsBox" )
{
	this->base = new stepsboxcontrol::StepsContainer( stepsControls, this->getFocusManager() );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );
}

StepsBox::~StepsBox()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
StepsBox::handleActionEvent( const ActionEvent& anEvent )
{
	bool result = false;
	return result;
}

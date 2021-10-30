/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Dimensions.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/SplitView.h"
#include "openocl/ui/SplitViewManager.h"

#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>

using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

unsigned int SplitViewManager::padding = 1;
unsigned int SplitViewManager::commandHeight = 30;

SplitViewManager::SplitViewManager( flags layoutHints )
{
	this->splitView = new SplitView( layoutHints );
	this->splitView->setEventMask( Event::MOUSE_EVENT | Event::MOTION_EVENT );
	
	this->outline = new Region();
	this->command = new Region();
	this->active  = new Region();

	this->splitView->addEventListener( *this );
}

SplitViewManager::~SplitViewManager()
{
	this->splitView->removeEventListener( *this );

	delete this->outline;
	delete this->command;
	delete this->active;
	
	delete this->splitView;
}

//------------------------------------------------------------------------------
//	public virtual methods (Event Listener)
//------------------------------------------------------------------------------

void
SplitViewManager::deliverEvent( const Event& anEvent )
{
	this->fireEvent( anEvent );
}

void
SplitViewManager::setContainingPanel( Panel& aPanel )
{
	this->Component::setContainingPanel( aPanel );
	this->splitView->setContainingPanel( aPanel );
}

void
SplitViewManager::doLayout( const GraphicsContext& gc )
{
	Region bounds = this->getBounds();

	Region outline;
	Region command;
	Region active;
	
	outline.x1     = bounds.x1 + padding;
	outline.x2     = bounds.x2 - padding;
	outline.width  = outline.x2 - outline.x1 + 1;
	outline.y1     = bounds.y1 + padding;
	outline.y2     = bounds.y2 - padding;
	outline.height = bounds.y2 - bounds.y1 + 1;
	
	command = outline;
	command.height = commandHeight;
	command.y2 = command.y1 + command.height - 1;

	active = outline;
	active.y1 = command.y2 + 1;
	active.height = active.y2 - active.y1 + 1;

	this->splitView->setBounds( active.x1, active.y1, active.width, active.height );
	this->splitView->Container::doLayout( gc );

	*this->outline = outline;
	*this->command = command;
	*this->active  = active;
}

void
SplitViewManager::processEvent( const Event& anEvent )
{
	int type = anEvent.getType();
	if ( (Event::MOUSE_EVENT & type) == Event::MOUSE_EVENT )
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
		if ( me.BUTTON1_RELEASE == ( me.BUTTON1_RELEASE & me.getButton() ) )
		{
			if ( this->command->contains( me.getX(), me.getY() ) )
			{
				this->splitView->nextComponent();
				this->splitView->getContainingPanel().redraw();
			}
		}
	}
	this->splitView->forwardEvent( anEvent );
}

//------------------------------------------------------------------------------------
//	public methods
//------------------------------------------------------------------------------------

void
SplitViewManager::add( Component& aComponent )
{
	this->splitView->add( aComponent );
}

void
SplitViewManager::remove( Component& aComponent )
{
	this->splitView->remove( aComponent );
}

//------------------------------------------------------------------------------------
//	public virtual constant methods
//------------------------------------------------------------------------------------

Dimensions
SplitViewManager::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d( width, height );
	return d;
}

bool
SplitViewManager::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	
	//Region command = *this->command;
	//Region active = *this->active;
	
	//gc.setForeground( Color::DARK_GREY );
	//gc.drawRectangleRegion( command );
	//gc.drawRectangleRegion( active );

	this->splitView->draw( gc );
	
	return draw_again;
}



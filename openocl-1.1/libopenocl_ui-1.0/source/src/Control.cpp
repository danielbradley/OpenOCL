/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Container.h"
#include "openocl/ui/Control.h"
#include "openocl/ui/FocusManager.h"
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Textual.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/RuntimeException.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/util/Sequence.h>
#include <cstdlib>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

Control::Control( const String& name ) : Component( name )
{
	this->disableForwardedEvents = false;
	this->disableLayout = false;
	this->disableDraw = false;
	this->resizeable = true;
	this->redrawAll = true;

	this->controlComponent = null;

	this->focusManager = new FocusManager( name );
	this->focusManager->addEventListener( *this ); 
	this->controlTitle = new String();
}

Control::~Control()
{
	this->focusManager->removeEventListener( *this );
	delete this->controlTitle;
	delete this->focusManager;
}

//-----------------------------------------------------------------------------
//	public virtual methods (EventListener)
//-----------------------------------------------------------------------------

void
Control::deliverEvent( const Event& anEvent )
{
	unsigned int type = anEvent.getType();
	
	if ( Event::ACTION_EVENT == type )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );

		if ( !this->handleActionEvent( ae ) )
		{
			this->fireEvent( anEvent );
		}
	}
	else if ( Event::FOCUS_EVENT == type )
	{
		const FocusEvent& focus_event = dynamic_cast<const FocusEvent&>( anEvent );
		if ( &anEvent.getEventSource() == this->focusManager )
		{
			int focus_type = focus_event.getFocusEventType();
			if ( FocusEvent::CIRCULATE_FORWARD == focus_type )
			{
				FocusEvent circulate_forward( *this, 0, FocusEvent::CIRCULATE_FORWARD );
				this->fireEvent( circulate_forward );
			}
			else if ( FocusEvent::CIRCULATE_BACKWARD == focus_type )
			{
				FocusEvent circulate_backward( *this, 0, FocusEvent::CIRCULATE_BACKWARD );
				this->fireEvent( circulate_backward );
			}
			else if ( FocusEvent::FOCUS_REQUESTED == focus_type )
			{
				FocusEvent focus_event( *this, 0, FocusEvent::FOCUS_REQUESTED );
				this->fireEvent( focus_event );
			}
		}
	}
	else if ( Event::CHANGE_EVENT == type )
	{
		this->setChanged( true );
		this->fireEvent( anEvent );
	}
}

//-----------------------------------------------------------------------------
//	public virtual methods (Component)
//-----------------------------------------------------------------------------

void
Control::doLayout( const GraphicsContext& gc )
{
	if ( this->disableLayout ) return;

	try {
		Component::doLayout( gc );
		if ( this->controlComponent )
		{
			this->controlComponent->doLayout( gc );
		}
	} catch ( RuntimeException* ex ) {
		ActionEvent ae( *this, 0, "RUNTIMEEXCEPTION.openocl::ui::Control", ex->getMessage() );
		this->deliverEvent( ae );
		delete ex;
	}
}

void
Control::forwardEvent( const Event& anEvent )
{
	//	ChangeEvents are forwarded to every
	//	component, while other events are processed by
	//	the Control by its FocusManager

	int type = anEvent.getType();

	//fprintf( stderr, "Control::forwardEvent: event type == 0x%x\n", type );

	bool change_event = (Event::CHANGE_EVENT == (Event::CHANGE_EVENT & type));
	bool action_event = (Event::ACTION_EVENT == (Event::ACTION_EVENT & type));
	bool focus_event  = (Event::FOCUS_EVENT  == (Event::FOCUS_EVENT  & type));

	if ( change_event )
	{
		const ChangeEvent& ce = dynamic_cast<const ChangeEvent&>( anEvent );
		if ( ChangeEvent::REDRAW_ALL == ce.getValue() )
		{
			//fprintf( stderr, "%s.Control::forwardEvent: redraw all\n", this->getName().getChars() );
			this->redrawAll = true;
		}
		this->setChanged( true );
		if ( this->controlComponent )
		{
			//fprintf( stderr, "%s.Control::forwardEvent()\n", this->getName().getChars() );
			this->controlComponent->forwardEvent( anEvent );
		}
	}
	else if ( action_event )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		const String& command = ae.getCommand();
		//fprintf( stderr, "Control::forwardEvent: %s\n", command.getChars() );

		if ( command.equals( "REFRESH.openocl::ui::Panel.CHECK_EVENTQUEUE" ) )
		{
			this->processEvent( anEvent );
		}
		else if ( this->controlComponent && command.startsWith( "REFRESH.openocl.ui.Panel" ))
		{
			this->controlComponent->forwardEvent( anEvent );
		}
	}
	else if ( focus_event )
	{
		this->focusManager->forwardEvent( anEvent );
	}
	else if ( !this->disableForwardedEvents )
	{
		this->processEvent( anEvent );
	}
}

/*
 *  A Control may receive REFRESH action events from its container
 *  in that case fire the event so that it is received by the View.
 */
void
Control::processEvent( const Event& anEvent )
{
#ifdef DEBUG_OPENOCL_UI_CONTROL_PROCESSEVENT
	fprintf( stderr, "Control::processEvent()\n" );
#endif

	flags type = anEvent.getType();
	if ( Event::ACTION_EVENT == type )
	{
		const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
		if ( ae.getCommand().equals( "REFRESH.openocl::ui::Panel.CHECK_EVENTQUEUE" ) )
		{
			this->fireEvent( anEvent );
		}
	}
	else if ( Event::KEY_EVENT == (Event::KEY_EVENT & type) )
	{
		this->focusManager->forwardEvent( anEvent );
	}
	else if ( this->controlComponent )
	{
		this->controlComponent->forwardEvent( anEvent );
	}
}

void
Control::setBounds( int x, int y, unsigned int width, unsigned int height )
{
#ifdef DEBUG_OPENOCL_UI_CONTROL
	int x2 = x + width - 1;
	int y2 = y + height - 1;
	fprintf( stderr, "%s.Control::setBounds: %i --> %i : %i ^ %i - ( %u, %u )\n", this->getName().getChars(),
			x, x2, y, y2, width, height );
#endif

	Component::setBounds( x, y, width, height );
	if ( this->controlComponent )
	{
		this->controlComponent->setBounds( x, y, width, height );
	}
}

void
Control::setContainingPanel( Panel& aPanel )
{
	Component::setContainingPanel( aPanel );
	if ( this->controlComponent )
	{
		this->controlComponent->setContainingPanel( aPanel );
	}
}

void
Control::resetContainingPanel()
{
	Component::resetContainingPanel();
	if ( this->controlComponent )
	{
		this->controlComponent->resetContainingPanel();
	}
}

//-----------------------------------------------------------------------------
//	public virtual methods (Control)
//-----------------------------------------------------------------------------

void
Control::refresh()
{}

//-----------------------------------------------------------------------------
//	public methods (Control)
//-----------------------------------------------------------------------------

void
Control::setControlComponent( Component& aComponent )
{
	try
	{
		this->controlComponent = &dynamic_cast<Component&>( aComponent );
		if ( this->hasContainingPanel() )
		{
			this->controlComponent->setContainingPanel( this->getContainingPanel() );
		}
	} catch ( ... )
	{
		fprintf( stderr, "Control::setControlComponent: unexpected exception, aborting!\n" );
		abort();
	}
}

void
Control::setControlTitle( const openocl::base::String& aTitle )
{
	delete this->controlTitle;
	this->controlTitle = new String( aTitle );
}

void
Control::setResizeable( bool isControlResizeable )
{
	this->resizeable = isControlResizeable;
}

FocusManager&
Control::getFocusManager()
{
	return *this->focusManager;
}

//-----------------------------------------------------------------------------
//	public virtual constant methods (Values)
//-----------------------------------------------------------------------------

bool
Control::hasKey( const openocl::base::String& name ) const
{
	bool has_key = false;
	try
	{
		this->findValue( name );
		has_key = true;
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
	return has_key;
}

Iterator*
Control::keys() const
{
	return null;
}

const String&
Control::findValue( const String& componentName, const String& defaultValue ) const
{
	const String* str;
	try
	{
		str = &this->findValue( componentName );
		if ( 0 == str->getLength() )
		{
			str = &defaultValue;
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
		str = &defaultValue;
	}
	return *str;
}

const String&
Control::findValue( const String& componentName ) const
throw (NoSuchElementException*)
{
	const String* str = null;
	
	if ( 0 == this->controlComponent->getName().compareTo( componentName ) )
	{
		const Textual* textual = dynamic_cast<const Textual*>( this->controlComponent );
		if ( textual )
		{
			str = &textual->getData();
		} else {
			throw new NoSuchElementException( componentName );
		}
	} else {
		const Container* container = dynamic_cast<const Container*>( this->controlComponent );
		if ( container )
		{
			str = &container->findValue( componentName );
		}
		const Control* control = dynamic_cast<const Control*>( this->controlComponent );
		if ( control )
		{
			str = &control->findValue( componentName );
		}
	}

	return *str;
}

//-----------------------------------------------------------------------------
//	public virtual constant methods (Component)
//-----------------------------------------------------------------------------

Dimensions
Control::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d;
	if ( this->controlComponent )
	{
		d = this->controlComponent->getPreferredDimensions( gc, width, height );
	}
	return d;
}

bool
Control::draw( GraphicsContext& gc ) const
{
	bool draw_again = false;
	
	if ( this->disableDraw ) return draw_again;

	if ( this->redrawAll )
	{
		Region bounds = this->getBounds();
		//Region clip = gc.getClipRegion();
		gc.setFill( gc.getBackground() );
		gc.drawFilledRectangleRegion( bounds );
		const_cast<Control*>( this )->redrawAll = false;
	}

	if ( this->hasChanged() )
	{
		if ( !this->controlComponent )
		{
			draw_again = Component::draw( gc );
		}
		else
		{
			draw_again = this->controlComponent->draw( gc );
		}
	}
	this->setChanged( false );
	
#ifdef DEBUG_OPENOCL_UI_CONTROL
	gc.setForeground( Color::BLUE );
	gc.drawRectangleRegion( this->getBounds() );
#endif
	
	return draw_again;
}

bool
Control::isResizeable() const
{
	return this->resizeable;
}

//-----------------------------------------------------------------------------
//	public constant methods (Control)
//-----------------------------------------------------------------------------

const String&
Control::getControlTitle() const
{
	return *this->controlTitle;
}

//-----------------------------------------------------------------------------
//	protected methods (Control)
//-----------------------------------------------------------------------------

void
Control::setDisableForwardedEvents()
{
	this->disableForwardedEvents = true;
}

void
Control::setDisableLayout()
{
	this->disableLayout = true;
}

void
Control::setDisableDraw()
{
	this->disableDraw = true;
}


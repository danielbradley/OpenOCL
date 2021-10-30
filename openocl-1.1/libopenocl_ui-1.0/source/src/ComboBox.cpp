/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/ComboBox.h"

#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/ComboGroup.h>
#include <openocl/ui/MenuItem.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/PopupMenu.h>
#include <openocl/ui/Region.h>
#include <openocl/util/Sequence.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;

unsigned int ComboBox::tabWidth = 20;

ComboBox::ComboBox( const String& aString ) : Textual( aString )
{
	this->setEdge( 10 );
	this->setMargin( 2 );
	this->setBorder( 0 );
	this->setPadding( 2 );

	this->setTextLayoutHints( Textual::MIDDLE | Textual::LEFT );
	this->setIndent( 5 );
	this->setSpacing( 0 );
	this->setRightIndent( this->tabWidth );

	this->tab     = new Region();
	this->currentSelection = new String( aString );

	this->comboGroup = new ComboGroup();
	this->comboGroup->addEventListener( *this );
	this->popup = null;
	this->active = false;
	this->selected = false;
	this->removePopup = false;
	this->popupWidth = 0;
	this->entries = 0;
}

ComboBox::~ComboBox()
{
	this->comboGroup->removeEventListener( *this );

	delete this->tab;
	delete this->currentSelection;

	//	Need to delete the popup before the combo group
	//	as it manipulates a reference to the combogroup
	//	during destruction.

	delete this->popup;
	delete this->comboGroup;
}

//-------------------------------------------------------------------
//	public virtual methods (EventListener)
//-------------------------------------------------------------------

void
ComboBox::deliverEvent( const Event& anEvent )
{
	if ( this->entries )
	{
		int event_type = anEvent.getType();
		switch ( event_type )
		{
		case Event::ACTION_EVENT:
			const ActionEvent& ae = dynamic_cast<const ActionEvent&>( anEvent );
	#ifdef DEBUG_OPENOCL_UI_COMBOBOX
			fprintf( stderr, "ComboBox::deliverEvent(): command=\"%s\" data=\"%s\"\n", ae.getCommand().getChars(), ae.getData().getChars() );
	#endif
			delete this->currentSelection;
			this->currentSelection = new String( ae.getData() );
			
			this->popup->setVisible( false );
			this->removePopup = true;
			
			this->fireChangeEvent();
			this->getContainingPanel().relayout(); //refresh();
			//this->getContainingPanel().refresh();
			break;
		}
	}
}

//-------------------------------------------------------------------
//	public virtual methods (Component)
//-------------------------------------------------------------------

void
ComboBox::processEvent( const Event& anEvent )
{
	if ( this->entries )
	{
		Region bounds = this->getBounds();
		int event_type = anEvent.getType();

		bool fire_change = false;

		switch ( event_type )
		{
		case Event::FOCUS_EVENT:
			{
				const FocusEvent& fe = dynamic_cast<const FocusEvent&>( anEvent );
				switch ( fe.getFocusEventType() )
				{
				case FocusEvent::TAKE_FOCUS:
					this->active = true;
					break;
				case FocusEvent::LOSE_FOCUS:
					this->hidePopup();
					this->active = false;
					this->selected = false;
				}
				fire_change = true;
			}		
			break;
		case Event::MOUSE_EVENT:
			{
				const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
				if ( bounds.contains( me.getX(), me.getY() ) )
				{
					if ( (MouseEvent::BUTTON1_PRESS == me.getButton()) && !this->selected )
					{
						FocusEvent ae( *this, 0, FocusEvent::FOCUS_REQUESTED );
						this->fireEvent( ae );

						this->active = true;
						this->selected = true;
						this->showPopup();
					}
					else if ( (MouseEvent::BUTTON1_PRESS == me.getButton()) && this->selected )
					{
						this->active = false;
						this->selected = false;
						this->hidePopup();
					}
				} else  if ( this->popup ) {
					this->active = false;
					this->hidePopup();
				}
				fire_change = true;
			}
			break;
		case Event::CLOSE_EVENT:
			{
				delete this->popup;
				this->popup = null;
			}
		}

		if ( fire_change )
		{
			this->fireChangeEvent();
			this->getContainingPanel().relayout();
		}
	}
}

void
ComboBox::setContainingPanel( Panel& aPanel )
{
	this->Textual::setContainingPanel( aPanel );

	if ( this->popup && !this->selected )
	{
		this->popup->remove( *this->comboGroup );
		delete this->popup;
		this->popup = null;
	}
}	

void
ComboBox::doLayout( const GraphicsContext& gc )
{
	this->Textual::doLayout( gc );

	Region outline = this->getEdgeRegion();
	Region text    = this->getTextRegion();
	Region tab;

	tab.y1 = outline.y1 + 1;
	tab.y2 = outline.y2 - 1;
	tab.height = tab.y2 - tab.y1 + 1;
	tab.width = tab.height;
	tab.x2 = outline.x2 - 1;
	tab.x1 = tab.x2 - tab.width + 1;

	if ( this->removePopup )
	{
		this->hidePopup();
		this->removePopup = false;
	}

	*this->tab = tab;
}

//-------------------------------------------------------------------
//	public methods
//-------------------------------------------------------------------

void
ComboBox::setDefaultEntry( const String& value )
{
	delete this->currentSelection;
	this->currentSelection = new String( value );
}

void
ComboBox::addEntry( const openocl::base::String& value )
{
	this->comboGroup->add( new MenuItem( value, "SELECT", value ) );
	this->entries++;
}

//-------------------------------------------------------------------
//	public virtual constant methods (Component)
//-------------------------------------------------------------------

Dimensions
ComboBox::getPreferredTextDimensions( const GraphicsContext& gc ) const
{
	Dimensions d = this->comboGroup->getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

	if ( this->entries )
	{
		TextExtents* extents = gc.measureTextExtents( this->getData() );
		d.width = Math::max( d.width, extents->width );
		d.height = extents->ascent + extents->descent;
		delete extents;
	} else {
		d.width = 1;
		d.height = 1;
	}

	return d;
}

bool
ComboBox::draw( GraphicsContext& gc ) const
{
	if ( this->entries )
	{
		Region bounds  = this->getBounds();
		Region box = this->getEdgeRegion();
		Region text    = this->getTextRegion();
		Region tab     = *this->tab;

		int min_width  = (this->getMargin()*2) + (this->getIndent() * 2) + 10;
		int min_height = 15;
		
		Color background( gc.getBackground() );
		
		bool changed = this->hasChanged();

		if ( changed && (bounds.width > min_width) && (bounds.height > min_height) )
		{
			gc.setForeground( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( bounds );

			gc.setForeground( Color::CANVAS );
			gc.drawFilledRectangleRegion( box );

			gc.setForeground( Color::TOOLBAR );
			gc.drawFilledRectangleRegion( tab );

			if ( this->selected || this->popup )
			{
				gc.setForeground( Color::SHADOW_GREY );
				gc.drawRectangleRegion( box );
				gc.drawLine( tab.x1, tab.y1, tab.x1, tab.y2 );
			}
			else if ( this->hasFocus() )
			{
				gc.setForeground( Color::SHADOW_GREY );
				gc.drawRectangleRegion( box );
				this->drawOutward3DBox( gc, tab, Color::SHINY, Color::SHADE );
			}
			else
			{
				this->drawOutward3DBox( gc, tab, Color::SHINY, Color::SHADE );
				this->drawInward3DBox( gc, box, Color::SHINY, Color::SHADE );
			}

			gc.setBackground( Color::CANVAS );

			if ( this->popup )
			{
				gc.setForeground( Color::TEXT );
			} else {
				gc.setForeground( Color::INACTIVE_TEXT );
			}
			gc.drawString( *this->currentSelection, text.x1, text.y2 );


			gc.setBackground( background );

	#ifdef DEBUG_OPENOCL_UI_COMBOBOX
			fprintf( stderr, "ComboBox::draw: currentSelection: %s\n", this->currentSelection->getChars() );
			this->Textual::draw( gc );
	#endif
		}

#ifdef DEBUG_OPENOCL_UI_TEXTUAL
		this->Textual::draw( gc );
#endif

		this->setChanged( false );
	}
	return false;
}

//------------------------------------------------------------------------------------
//	private methods
//------------------------------------------------------------------------------------

void
ComboBox::showPopup()
{
	#ifdef DEBUG_OPENOCL_UI_COMBOBOX
	fprintf( stderr, "ComboBox::showPopup()\n" );
	#endif

	unsigned int edge = this->getEdge();
	
	Region bounds = this->getBounds();
	Region box = this->getEdgeRegion();

	if ( !this->popup )
	{
		this->selected = true;
		if ( this->hasContainingPanel() )
		{
			this->popup = new PopupMenu( this->getContainingPanel() );
			this->popup->setName( "popup" );
			this->popup->add( *this->comboGroup );
		}

		if ( this->popup )
		{
			Coordinates c = this->getAbsoluteCoordinates();

			int x = c.x + edge;
			int y = c.y + bounds.height - edge - 1;
		
			//unsigned int width  = box.width - this->tab->width - 2;
			unsigned int width  = Math::max( box.width, this->popup->getPreferredWidth() );
			unsigned int height = this->popup->getPreferredHeight();

			this->popup->requestGeometry( x, y, width, height, 0 );
			//this->popup->requestGeometry( ax + padding + 1, ay + padding + this->outline->height, width, height, 0 );

			//this->popup->toFront();
			this->popup->show( Panel::SHOW );
			this->popup->getTopPanel().relayout();
			//this->popup->setVisible( true );
		}
	}
}

void
ComboBox::hidePopup()
{
	#ifdef DEBUG_OPENOCL_UI_COMBOBOX
	fprintf( stderr, "ComboBox::hidePopup()\n" );
	#endif
	if ( this->popup )
	{
		this->selected = false;
		this->popup->setVisible( false );
		
		this->popup->remove( *this->comboGroup );
		delete this->popup;
		this->popup = null;

		this->fireChangeEvent();
		this->getContainingPanel().relayout();
	}
}

const String&
ComboBox::getData() const
{
	return *this->currentSelection;
}

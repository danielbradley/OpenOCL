/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Icon.h"

#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/OffscreenImage.h"
#include "openocl/ui/OffscreenImageManager.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/PopupMenu.h"
#include "openocl/ui/Region.h"
#include "openocl/ui/IconText.h"
#include "openocl/ui/Visual.h"
#include "openocl/base/Math.h"
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/imaging/Image.h>
#include <openocl/imaging/TextExtents.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::io;
using namespace openocl::ui;

unsigned int Icon::size = 300;

Icon::Icon( const String& name ) : Selector( name )
{
	this->init();
}

Icon::Icon( const String& name, const String& command )
: Selector( name, command )
{
	this->init();
}

Icon::Icon( const String& name, const String& command, const String& label )
: Selector( name, command, label )
{
	this->init();
}

void
Icon::init()
{
	this->setEventMask( Event::MOUSE_EVENT | Event::MOTION_EVENT );
	this->setMargin( 2 );
	this->setIndent( 0 );
	this->setTextLayoutHints( Textual::MIDDLE | Textual::CENTERED );
	this->setBounds( 0, 0, 100, 100 );

	this->picture = new Region();
	this->text    = new Region();

	this->minPreferredWidth = 100;
	this->maxPreferredWidth = 150;

	this->imageId = null;
	this->image   = null;

	this->iconText = new IconText( this->getName() );
	this->iconText->setTextLayoutHints( Textual::MIDDLE | Textual::CENTERED );
	this->iconText->setText( this->getLabel() );
	
	this->selected  = false;
	this->hoverOver = false;
	this->pressed   = false;
}

Icon::~Icon()
{
	delete this->picture;
	delete this->text;

	delete imageId;

	delete this->iconText;
}

void
Icon::deliverEvent( const Event& anEvent )
{
	this->fireEvent( anEvent );
}

void
Icon::processEvent( const Event& anEvent )
{
	bool fire_double = false;
	bool fire_single = false;
	bool fire_change = false;
	bool fire_action = false;

	flags type = anEvent.getType();

	switch ( type )
	{
/*
	case Event::MOTION_EVENT:
		{
			const MotionEvent& me = dynamic_cast<const MotionEvent&>( anEvent );
			if ( &me )
			{
				unsigned int me_x = me.getX();
				unsigned int me_y = me.getY();

				bool previous = this->hoverOver;
				this->hoverOver = this->contains( me_x, me_y );
				
				if ( previous != hoverOver ) fire_change = true;
				
				this->pressed &= this->hoverOver;
		
				if ( this->clickX || this->clickY )
				{
					this->preferredX = this->lastPreferredX + (me_x - this->clickX);
					this->preferredY = this->lastPreferredY + (me_y - this->clickY);
					//this->setBounds( this->preferredX, this->preferredY, this->getWidth(), this->getHeight() );
					fire_change = true;
				}
			}
		}
		break;
*/
	case Event::MOUSE_EVENT:
		{
			const MouseEvent&  mo = dynamic_cast<const MouseEvent&>( anEvent );
			if ( &mo )
			{
				int   mo_x = mo.getX();
				int   mo_y = mo.getY();
				flags mo_button = mo.getButton();
	
				if ( mo.BUTTON1_PRESS == (mo.BUTTON1_PRESS & mo_button ) )
				{
					if ( this->contains( mo_x, mo_y ) )
					{
						this->pressed = true;

						if ( mo.getClickCount() > 1 )
						{
							this->pressed = false;
							fire_double = true;
							fire_action = true;
						}
						
						fire_change = true;
					}
				}
				else if ( mo.BUTTON1_RELEASE == (mo.BUTTON1_RELEASE & mo_button ) )
				{
					if ( this->pressed && this->contains( mo_x, mo_y ) )
					{
						fire_single = true;
						fire_action = true;
					}
					this->pressed = false;
					fire_change = true;
				}
			}
		}
	}

	//
	//	Icons dont change when clicked so why are we sending a redraw event
	//
	//if ( fire_change )
	//{
	//	this->fireChangeEvent();
	//	const_cast<Panel&>( this->getContainingPanel() ).redraw();
	//}

	if ( fire_action )
	{
#ifdef DEBUG_OPENOCL_UI_ICON
		fprintf( stderr, "Icon::processEvent: name: %s action: %s label: %s data: %s\n",
				this->getName().getChars(), this->getAction().getChars(), this->getLabel().getChars(),
				this->getData().getChars() );
#endif
		if ( fire_double )
		{
			//fprintf( stderr, "Icon::processEvent: SELECTED.openocl::ui::Icon.DOUBLE_CLICKED\n" );
			this->fireSelectorEvent( "openocl::ui::Icon.DOUBLE_CLICKED" );
		}
		else if ( fire_single )
		{
			//fprintf( stderr, "Icon::processEvent: SELECTED.openocl::ui::Icon.CLICKED\n" );
			this->fireSelectorEvent( "openocl::ui::Icon.CLICKED" );
		}
	}
}

void
Icon::doLayout( const openocl::ui::GraphicsContext& gc )
{
	unsigned int margin = this->getMargin();

	Region bounds  = this->getBounds();
	Region outline = this->getBox();
	Region icon;
	Region picture;
	Region text;

	icon.x1 = outline.x1 + margin;
	icon.x2 = outline.x2 - margin;
	icon.width = icon.x2 - icon.x1 + 1;
	icon.y1 = outline.y1 + margin;
	icon.y2 = outline.y2 - margin;
	icon.height = icon.y2 - icon.y1 + 1;

	picture.x1 = icon.x1;
	picture.x2 = icon.x2;
	picture.width = icon.width;

	if ( this->imageId )
	{
		try
		{
			if ( !this->image )
			{
				OffscreenImageManager& oim = gc.getVisual().getOffscreenImageManager();
				this->image = &oim.findImage( gc, *this->imageId );
			}
			picture.y1 = icon.y1;
			picture.height = this->image->getDrawableHeight();
			picture.y2 = picture.y1 + picture.height - 1;
		} catch ( NoSuchElementException* ex ) {
			delete ex;
			picture.y1 = icon.y1;
			picture.y2 = icon.y1;
			picture.height = 1;
		}
	} else {
		picture.y1 = icon.y1;
		picture.height = 32;
		picture.y2 = picture.y1 + picture.height - 1;
	}

	Dimensions dim;
	if ( this->iconText )
	{
		dim = this->iconText->getPreferredDimensions( gc, icon.width, icon.height );
	}

	text.x1 = icon.x1;
	text.x2 = icon.x2;
	text.width = text.x2 - text.x1 + 1;

	if ( (0 < this->getLabel().getLength()) )
	{
		text.y1 = picture.y2 + 1;
		text.y2 = icon.y2;
		text.height = text.y2 - text.y1 + 1;
	} else {
		text.y1 = icon.y2;
		text.y2 = icon.y2;
		text.height = 1;
	}

	if ( this->iconText )
	{
		this->iconText->setBounds( text.x1, text.y1, text.width, text.height );
		this->iconText->doLayout( gc );
	}
	
	*this->picture = picture;
	*this->text    = text;
}

void
Icon::setImageId( const openocl::base::String& anImageId )
{
	delete this->imageId;
	this->imageId = new String( anImageId );
}

void
Icon::setLabel( const String& aLabel )
{
	this->setData( aLabel );
	if ( this->iconText )
	{
		this->iconText->setText( aLabel );
	}
}

void
Icon::setMaxPreferredWidth( unsigned int width )
{
	this->maxPreferredWidth = width;
}

void
Icon::setMinPreferredWidth( unsigned int width )
{
	this->minPreferredWidth = width;
}

Dimensions
Icon::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions dim;

	unsigned int margin = this->getMargin();
	unsigned int space  = this->getPadding()*2 + margin*2;
	unsigned int available_width = Math::min( width, this->maxPreferredWidth ) - space;

	if ( this->iconText )
	{
		dim = this->iconText->getPreferredDimensions( gc, available_width, height );
	}
	
	if ( this->imageId && !this->image ) this->obtainIconImage( gc );
	if ( this->image )
	{
		dim.width   = Math::max( dim.width, this->image->getDrawableWidth() );
		dim.width   = Math::min( dim.width, available_width );
		dim.height += this->image->getDrawableHeight();
	} 

	if ( this->image && (0 < this->getLabel().getLength() ) )
	{
		dim.height += margin;
	}
	
	dim.width  += space;
	dim.height += space;

	dim.width = Math::max( dim.width, this->minPreferredWidth );

//	dim.width  = 100;
//	dim.height = 100;

	return dim;
}

bool
Icon::draw( GraphicsContext& gc ) const
{
	bool paint_background = true;
	bool draw_image = true;
	//bool draw_hover = false;
	bool draw_text  = true;

	if ( this->hasChanged() )
	{
		Region bounds  = this->getBounds();
		Region inner_bounds = bounds.shrunkBy( 1 );
		//Region debug_text = bounds.shrunkBy( 10 );
		Region picture = *this->picture;
		Region text    = *this->text;

		Color original_background( gc.getBackground() );

		if ( paint_background )
		{
			gc.setFill( original_background );
			gc.drawFilledRectangleRegion( bounds );
		}

		//gc.setForeground( Color::BLACK );
		//gc.drawBoundedString( this->getLabel(), debug_text );
		
		//gc.setForeground( Color::BLUE );
		//gc.drawRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );

		//gc.setForeground( Color::RED );
		//gc.drawRectangle( outline.x1, outline.y1, outline.width, outline.height );

		//gc.setForeground( Color::RED );
		//gc.drawRectangle( icon.x1, icon.y1, icon.width, icon.height );

		//gc.setForeground( Color::GREEN );
		//gc.drawRectangle( picture.x1, picture.y1, picture.width, picture.height );

		//gc.setForeground( Color::BLUE );
		//gc.drawRectangle( text.x1, text.y1, text.width, text.height );

		if ( draw_image && this->image )
		{
			int cx = picture.x1 + (picture.width/2);
			int cy = picture.y1 + (picture.height/2);
		
			int p_width = this->image->getDrawableWidth();
			int p_height = this->image->getDrawableHeight();
		
			int px = cx - (p_width / 2);
			int py = cy - (p_height / 2);
		
			gc.drawImage( *this->image, px, py );

#ifdef DEBUG_OPENOCL_UI_ICON
			fprintf( stderr, "Icon::draw()\n" );
			gc.setForeground( Color::RED );
			gc.drawRectangle( px, py, p_width, p_height );
#endif
			//gc.copyArea( *this->image, 0, 0, p_width, p_height, px, py );
		}

/*	
		Region inner = outline.shrunkBy( 1 );
		if ( draw_hover && this->hoverOver )
		{
			Region inner = outline.shrunkBy( 1 );
			
			if ( this->pressed )
			{
				this->drawInward3DBox( gc, outline, Color::SHINE2, Color::SHADE1 );
			}
		}
*/
	
		//	Draw text
		
		if ( draw_text && (0 < this->getLabel().getLength()) )
		{
			if ( this->iconText )
			{
				this->iconText->draw( gc );
			} else {
				gc.setForeground( Color::BLACK );
				gc.drawBoundedString( this->getLabel(), text );
			}
		}

		//	gc.setForeground( Color::RED );
		//	gc.drawRectangle( bounds.x1, bounds.y1, bounds.width, bounds.height );
		//	gc.setForeground( Color::RED );
		//	gc.drawRectangle( outline.x1, outline.y1, outline.width, outline.height );
		//	gc.setForeground( Color::GREEN );
		//	gc.drawRectangle( picture.x1, picture.y1, picture.width, picture.height );
		//	gc.setForeground( Color::BLUE );
		//	gc.drawRectangle( text.x1, text.y1, text.width, text.height );
	
		this->setChanged( false );
	}
	
	return false;
}

int
Icon::getPreferredX() const
{
	return this->getBounds().x1;
}

int
Icon::getPreferredY() const
{
	return this->getBounds().y1;
}

void
Icon::obtainIconImage( const GraphicsContext& gc ) const
{
	if ( this->imageId && !this->image )
	{
		try
		{
			OffscreenImageManager& oim = gc.getVisual().getOffscreenImageManager();
			const_cast<Icon*>( this )->image = &oim.findImage( gc, *this->imageId );
		} catch ( NoSuchElementException* ex ) {
			delete ex;
		}
	}
}


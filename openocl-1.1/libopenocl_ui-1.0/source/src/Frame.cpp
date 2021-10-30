/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Frame.h"

#include <openocl/base/Math.h>
#include <openocl/base/String.h>
//#include <openocl/mvc/Controller.h>
//#include <openocl/mvc/View.h>
#include "openocl/ui/GraphicsContext.h"
#include "openocl/ui/Panel.h"
#include "openocl/ui/Visual.h"

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;

Frame::Frame( Panel* aPanel )
{
	this->panel = aPanel;
	//this->panel->setName( "top_panel" );
	this->panel->addEventListener( *this );
}

Frame::~Frame()
{
	this->panel->removeEventListener( *this );
	delete this->panel;
}

void
Frame::deliverEvent( const Event& anEvent )
{}


void
Frame::requestGeometry( int x, int y, unsigned int width, unsigned int height, unsigned int border )
{
	this->panel->requestGeometry( x, y, width, height, border );
}

void
Frame::requestPosition( int x, int y )
{
	this->panel->requestPosition( x, y );
}

void
Frame::requestSize( unsigned int width, unsigned int height )
{
	this->panel->requestSize( width, height );
}

void
Frame::positionAndResize( int x, int y )
{
	unsigned int width  = this->getPreferredWidth();
	unsigned int height = this->getPreferredHeight();

	this->requestGeometry( x, y, width, height, 0 );
}

void
Frame::positionAndResize( int x, int y, unsigned int maxWidth, unsigned int maxHeight )
{
	unsigned int width  = Math::min( maxWidth, this->getPreferredWidth() );
	unsigned int height = Math::min( maxHeight, this->getPreferredHeight() );

	this->requestGeometry( x, y, width, height, 0 );
	Dimensions d;
	d.width = width;
	d.height = height;
}

void
Frame::resize( unsigned int maxWidth, unsigned int maxHeight )
{
	unsigned int width  = Math::min( maxWidth, this->getPreferredWidth() );
	unsigned int height = Math::min( maxHeight, this->getPreferredHeight() );

	this->requestSize( width, height );
}

void
Frame::setAlwaysOnTop( bool alwaysOnTop )
{
	this->panel->setAlwaysOnTop( alwaysOnTop );
}

void
Frame::setName( const String& aName )
{
	this->panel->setName( aName );
}


void
Frame::show( int state )
{
	this->panel->show( state );
}

void
Frame::setVisible( bool aValue )
{
	if ( aValue )
	{
		this->panel->show( Panel::SHOW );
		//  this->panel->map();
	} else {
		this->panel->show( Panel::HIDDEN );
		//  this->panel->unmap();
	}
}

void
Frame::toFront()
{
	this->panel->raise();
}

//void
///Frame::add( View& aView )
//{
//	Object& obj = aView.getComponent();
//	Component* com = dynamic_cast<Component*>( &obj );
//	if ( com )
//	{
//		this->getTopPanel().add( *com );
//	}
//}

void
Frame::add( Component& aViewComponent )
{
	this->getTopPanel().add( aViewComponent );
}

void
Frame::remove( Component& aViewComponent )
{
	this->getTopPanel().remove( aViewComponent );
}

Panel&
Frame::getTopPanel() const
{
	return *this->panel;
}

//Controller&
//Frame::getController() const
//{
//	return this->panel->getVisual().getController();
//}

unsigned int
Frame::getPreferredWidth()
{
	GraphicsContext& gc = this->getTopPanel().getGraphicsContext();
	Dimensions dim = this->getTopPanel().getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

	if ( dim.width > 1280 )
	{
		dim.width = 1280;
	}
	if ( dim.width < 0 )
	{
		dim.width = 100;
	}
	return dim.width;
}


unsigned int
Frame::getPreferredHeight()
{
	GraphicsContext& gc = this->getTopPanel().getGraphicsContext();
	Dimensions dim = this->getTopPanel().getPreferredDimensions( gc, 0xFFFF, 0xFFFF );

	if ( dim.height > 1024 )
	{
		dim.height = 1024;
	}
	if ( dim.height < 0 )
	{
		dim.height = 100;
	}		
	return dim.height;
}

bool
Frame::isVisible() const
{
	return this->panel->isVisible();
}

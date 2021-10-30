/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/RuntimeSummaryItem.h"
#include "openocl/ui/GraphicsContext.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/Math.h>
#include <openocl/base/Runtime.h>
#include <openocl/base/String.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/util/Sequence.h>
#include <openocl/ui/Panel.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

RuntimeSummaryItem::RuntimeSummaryItem() : MenuItem( "openocl::ui::RuntimeSummaryItem" )
{
	this->values = new long long int[500];
	this->last  = 0;
	this->clocked = false;
	this->objects = 0;
	this->news    = 0;
	this->cmem    = 0;
	this->reset = true;
}

RuntimeSummaryItem::~RuntimeSummaryItem()
{
	delete this->values;
}

void
RuntimeSummaryItem::processEvent( const Event& anEvent )
{
	if ( Event::MOUSE_EVENT == anEvent.getType() )
	{
		const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
		if ( this->contains( me.getX(), me.getY() ) )
		{
			this->reset = true;
		}
	}

	int object_count = this->getObjectCount();
	if ( object_count != this->values[this->last - 1] )
	{
		this->values[this->last++] = object_count;
	}

	if ( this->last == 500 )
	{
		this->last = 0;
		this->clocked = true;
	}
	
	this->fireChangeEvent();
	this->getContainingPanel().redraw();
}

Dimensions
RuntimeSummaryItem::getPreferredDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
{
	Dimensions d = this->MenuItem::getPreferredDimensions( gc, width, height );
	d.width = Math::min( width, 800 );
	return d;
}

bool
RuntimeSummaryItem::draw( GraphicsContext& gc ) const
{
	if ( this->reset )
	{
		const_cast<RuntimeSummaryItem*>( this )->objects = this->getObjectCount();
		const_cast<RuntimeSummaryItem*>( this )->news    = Runtime::getAllocationCount();
		const_cast<RuntimeSummaryItem*>( this )->cmem    = Runtime::getCRuntimeAllocationCount();
		const_cast<RuntimeSummaryItem*>( this )->reset = false;
	}

	long long int object_count = this->getObjectCount() - this->objects;
	long long int news_count = Runtime::getAllocationCount() - this->news;
	long long int cmem_count = Runtime::getCRuntimeAllocationCount() - this->cmem;

	FormattedString str( "o: %lli n: %lli r: %lli", object_count, news_count, cmem_count );
	Region bounds = this->getBounds();
	Region box    = this->getBox();

	gc.setFill( Color::DIALOG );
	gc.drawFilledRectangleRegion( bounds );
	gc.setForeground( Color::RED );
	gc.drawBoundedString( str, box );

	int indent = 200;
	if ( !clocked )
	{
		unsigned int max = this->last;
		for ( unsigned int i=0; i < max; i++ )
		{
			long long int count = this->values[i];
			long long int count2 = count / 1000;
		
			gc.drawLine( indent + i, box.y2, indent + i, box.y2 - count2 );
		}
	} else {
		gc.setForeground( Color::RED );
		unsigned int max = 500;
		for ( unsigned int i=0; i < max; i++ )
		{
			long long int count = this->values[i];
			long long int count2 = count / 1000;
		
			gc.drawLine( indent + i, box.y2, indent + i, box.y2 - count2 );
		}
		gc.setForeground( Color::GREY );
		gc.drawLine( indent + this->last, box.y2, indent + this->last, box.y1 );
	}
	

	return false;
}

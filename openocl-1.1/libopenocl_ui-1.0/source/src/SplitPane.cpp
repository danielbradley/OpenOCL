/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/SplitPane.h"

#include "openocl/ui/ColumnLayout.h"
#include "openocl/ui/Panel.h"
#include <openocl/base/FormattedString.h>
#include <openocl/base/Iterator.h>
#include <openocl/base/String.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/MotionEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/base/event/PointerEvent.h>
#include <openocl/io/IO.h>
#include <openocl/io/Printer.h>
#include <openocl/util/Sequence.h>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::io;
using namespace openocl::ui;
using namespace openocl::util;

SplitPane::SplitPane()
{
	this->setLayout( new ColumnLayout() );
	this->offsetX = 0;
	this->offsetY = 0;
	this->split = 10;
}

SplitPane::SplitPane( unsigned int width, unsigned int height )
: Pane( width, height )
{
	this->setLayout( new ColumnLayout() );
	this->offsetX = 0;
	this->offsetY = 0;
	this->split = 10;
}

SplitPane::~SplitPane()
{
}

void
SplitPane::setName( const String& name )
{
	this->Component::setName( name );
	
	Iterator* it = this->getComponents().iterator();
	while ( it->hasNext() )
	{
		Component& com = const_cast<Component&>( dynamic_cast<const Component&>( it->next() ) );
		com.setName( FormattedString( "%s.%s", name.getChars(), com.getName().getChars() ) );
	}
}

void
SplitPane::doLayout( const GraphicsContext& gc )
{
	//IO::err().printf( "SplitPane::doLayout()\n" );
	this->Container::doLayout( gc );
	
	try
	{
		if ( this->getComponents().getLength() )
		{
			const Pane& pane = dynamic_cast<const Pane&>( this->getComponents().elementAt( 0 ) );
			this->split = this->getX() + pane.getPreferredDimensions( gc, 0xFFFF, 0xFFFF ).width;
		}
	} catch ( NoSuchElementException* ex ) {
		delete ex;
	}
}

//bool
//SplitPane::draw( GraphicsContext& gc ) const
//{
//	bool draw_again = false;
//
//	return draw_again;
//}

void
SplitPane::processEvent( const Event& anEvent )
{
	bool update = false;

	flags type = anEvent.getType();
	switch ( Event::POINTER_EVENT & type )
	{
	case Event::POINTER_EVENT:
		{
			const PointerEvent& pe = dynamic_cast<const PointerEvent&>( anEvent );
			if ( &pe )
			{
				int x = this->getX();
				int y = this->getY();

				int event_x = pe.getX();
				int event_y = pe.getY();

				int left  = this->split - 2;
				int right = this->split + 2;

				switch ( type )
				{
				case Event::MOUSE_EVENT:
					{
						const MouseEvent& me   = dynamic_cast<const MouseEvent&>( anEvent );
						if ( &me )
						{
							flags button = me.getButton();
							if ( me.BUTTON1_PRESS == (me.BUTTON1_PRESS & button) )
							{
								if ( (left < event_x) && ( event_x < right ) )
								{
									this->offsetX = x - event_x;
									this->offsetY = y - event_y;
								}
							}
							else if ( me.BUTTON1_RELEASE == (me.BUTTON1_RELEASE & button) )
							{
								this->offsetX = 0;
								this->offsetY = 0;
							}
						}
					}
					break;
				case Event::MOTION_EVENT:
					{
						const MotionEvent& mo  = dynamic_cast<const MotionEvent&>( anEvent );
						if ( &mo && ( this->offsetX || this->offsetY ) )
						{
							this->split = event_x;
							try
							{
								Pane& pane = const_cast<Pane&>( dynamic_cast<const Pane&>( this->getComponents().elementAt( 0 ) ) );
								pane.setPaneWidth( event_x - x );
								this->setBoundsChanged( true );
								update = true;
							} catch ( NoSuchElementException* ex ) {
								delete ex;
							}
						}
					}
					break;
				};
			}
		}
		break;
	};
	
	if ( update )
	{
		const_cast<Panel&>( this->getContainingPanel() ).relayout();
	}
}

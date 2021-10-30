/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/LocationBar.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/FocusEvent.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/base/event/MouseEvent.h>
#include <openocl/imaging/Color.h>
#include <openocl/io/Directory.h>
#include <openocl/io/Path.h>
#include <openocl/ui/FocusManager.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/util/Sequence.h>

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
		namespace locationbarcontrol {
		
class LocationField : public openocl::ui::TextField
{
public:
	LocationField( const openocl::base::String& name )
	: TextField( name )
	{
		this->setEditMode( false );
	}

	void processEvent( const Event& anEvent )
	{
		int type = anEvent.getType();

/*
		if ( Event::MOUSE_EVENT == type )
		{
			const MouseEvent& me = dynamic_cast<const MouseEvent&>( anEvent );
			int me_x = me.getX();
			int me_y = me.getY();
			this->TextField::processEvent( anEvent );
		}
		else
*/
		if ( Event::KEY_EVENT == (Event::KEY_EVENT & type) )
		{
			if ( this->isEditMode() )
			{
				//fprintf( stderr, "LocationBar::processEvent: EDIT MODE\n" );
				const KeyEvent& ke = dynamic_cast<const KeyEvent&>( anEvent );
				unsigned int key = ke.getKey();

				switch ( KeyEvent::PRINTABLE_MASK & key )
				{
				case 0:
					break;
				case KeyEvent::FORWARDSLASH:
				case KeyEvent::BACKSLASH:
					{
						this->TextField::processEvent( anEvent );
						this->checkLocation( this->getData() );
					}
					break;
				case KeyEvent::TAB:
					{
						this->guessLocation( this->getData() );
						this->checkLocation( this->getData() );
					}
					break;
				case KeyEvent::BACKSPACE:
					{
						this->TextField::processEvent( anEvent );
						this->checkLocation( this->getData() );
						////this->fire = true;
					}
					break;
				case KeyEvent::ESC:
					{
						this->setEditMode( false );
						this->fireChangeEvent();
						this->getContainingPanel().redraw();
					}
					break;
				default:
					this->TextField::processEvent( anEvent );
					this->checkLocation( this->getData() );
				}
					
				switch ( KeyEvent::UNPRINTABLE_MASK & key )
				{
				case 0:
					break;
				case KeyEvent::LEFT:
				case KeyEvent::RIGHT:
					this->TextField::processEvent( anEvent );
				}

				this->fireChangeEvent();
				this->getContainingPanel().relayout();  // required because of change in text box.

			} else {
				const KeyEvent& ke = dynamic_cast<const KeyEvent&>( anEvent );
				if ( KeyEvent::RETURN == ke.getKey() )
				{
					this->setEditMode( true );
					this->fireChangeEvent();
					this->getContainingPanel().redraw();
				}
			}
		} else {
			this->TextField::processEvent( anEvent );
		}

		if ( false )//this->fire )
		{
			//	XXX	Need to test for empty path here, else instantiating path
			//		is relative to current working directory.
						
			Path path( this->getData() );
			switch ( path.getType() )
			{
			case Path::NONE:
				{
					ActionEvent ae( *this, 0, "CHANGED_DIR.openocl::controls::LocationBar", path.getDirname() );
					fireEvent( ae );
				}
				break;
			case Path::DIRECTORY:
				{
					ActionEvent ae( *this, 0, "CHANGED_DIR.openocl::controls::LocationBar", path.getAbsolute() );
					fireEvent( ae );
				}
				break;
			default:
				{
					ActionEvent ae( *this, 0, "SELECTED_FILE.openocl::controls::LocationBar", path.getAbsolute() );
					fireEvent( ae );
				}
			}
	#ifdef DEBUG_OPENOCL_CONTROLS_LOCATIONBAR
			fprintf( stderr, "LocationBar::processEvent: %s\n", path.getDirname().getChars() );
	#endif
		}
	}

	bool draw( GraphicsContext& gc ) const
	{
		if ( this->isEditMode() )
		{
			this->TextField::draw( gc );
		}
		else
		{
			Region bounds = this->getBounds();
			Region box    = this->getEdgeRegion();
			Region text   = this->getTextRegion();

			Color previous_background( gc.getBackground().getColor() );
			Color background( Color::CANVAS );

			//	Draw canvas background
			gc.setFill( background );
			gc.drawFilledRectangle( box.x1, box.y1, box.width, box.height );

			if ( this->hasFocus() )
			{
				gc.setForeground( Color::SHADOW_GREY );
				gc.drawRectangleRegion( box );
			} else {
				this->drawInward3DBox( gc, box, Color::SHINY, Color::SHADE );
			}
			gc.setForeground( Color::INACTIVE_TEXT );
			gc.setBackground( background );
			gc.drawBoundedString( this->getData(), text, 0 );

	#ifdef DEBUG_OPENOCL_UI_TEXTFIELD
			this->Textual::draw( gc );
	#endif
			gc.setBackground( previous_background );
		}
	
		this->setChanged( false );
		return false;
	}

	//	Check the passed location if it is a directory fire an event.
	//

	void checkLocation( const openocl::base::String& aLocation )
	{
		Path path( aLocation );
		int type = path.getType();
		char fs = Path::getFileSeparator();

		switch ( type )
		{
		case Path::DIRECTORY:
			{
				StringBuffer sb;
				sb.append( path.getViewable() );
				if ( fs != sb.charAt( sb.getLength() - 1 ) )
				{
					sb.append( Path::getFileSeparator() );
				}
				ActionEvent ae( *this, 0, "CHANGE_DIR.openocl::controls::LocationBar.CHANGED_DIR", path.getAbsolute() );
				fireEvent( ae );
			}
			break;
		case Path::FILE:
			{
				ActionEvent ae( *this, 0, "SELECT_FILE.openocl::controls::LocationBar.SELECTED_FILE", path.getAbsolute() );
				fireEvent( ae );
			}
			break;
		default:
			{
				const String& dirname = path.getDirname();
				if ( dirname.equals( "" ) )
				{
					ActionEvent ae( *this, 0, "CHANGE_DIR.openocl::controls::LocationBar.CHANGED_DIR", "/" );
					fireEvent( ae );
				}
				else
				{
					ActionEvent ae( *this, 0, "CHANGE_DIR.openocl::controls::LocationBar.CHANGED_DIR", dirname );
					fireEvent( ae );
				}
			}
		}
	}

	void guessLocation( const openocl::base::String& aPartialLocation )
	{
		unsigned int length = aPartialLocation.getLength();
		
		Path partial_path( aPartialLocation );
		int type = partial_path.getType();

		switch ( type )
		{
		case Path::DIRECTORY:
			{
				StringBuffer sb;
				sb.append( aPartialLocation );
				char tmp = aPartialLocation.charAt( length - 1 );
				if ( ('/' != tmp) && ('\\' != tmp) )
				{
					sb.append( Path::getFileSeparator() );
				}
				this->setData( sb.asString() );

				//ActionEvent ae( *this, 0, "SYNC_DIR.openocl::controls::LocationBar.CHANGED_DIR", sb.asString() );
				//fireEvent( ae );

				//this->fire = true;
			}
			break;
		case Path::NONE:
			{
				const String& parent_name = partial_path.getDirname();
				Path parent_path( parent_name );
				int parent_type = parent_path.getType();
				switch ( parent_type )
				{
				case Path::DIRECTORY:
					{
						Directory parent_dir( parent_path );
						parent_dir.open();
						String* filter = new String( partial_path.getBasename() );
			
	#ifdef DEBUG_OPENOCL_CONTROLS_LOCATIONBAR
						fprintf( stderr, "parent:  %s\n", parent_name.getChars() );
						fprintf( stderr, "partial: %s\n", aPartialLocation.getChars() );
						fprintf( stderr, "filter:  %s\n", filter->getChars() );
	#endif		
						Sequence* files = parent_dir.filenamesStartingWith( *filter );

						if ( 1 == files->getLength() )
						{
							const String& filename = dynamic_cast<const String&>( files->elementAt( 0 ) );
							StringBuffer sb;
							sb.append( parent_path.getViewable() );
							if ( Path::getFileSeparator() != sb.charAt( sb.getLength() - 1 ) )
							{
								sb.append( Path::getFileSeparator() );
							}
							sb.append( filename );
							this->setData( sb.asString() );
							//this->fire = true;

							//Path possible_dir( sb.asString() );
							//if ( Path::DIRECTORY == possible_dir.getType() )
							//{
							//	sb.append( '/' );
							//	ActionEvent ae( *this, 0, "openocl::controls::LocationBar::CHANGED_DIR", sb.asString() );
							//	this->fireEvent( ae );
							//}
						}
						//else if ( 1 < files->getLength() ) {
						//	StringBuffer sb;
						//	sb.append( '{' );
						//	Iterator* it = files->iterator();
						//	while ( it->hasNext() )
						//	{
						//		const String& str = dynamic_cast<const String&>( it->next() );
						//		sb.append( str );
						//		sb.append( ", " );
						//	}
						//	delete it;
						//	sb.removeLast();
						//	sb.removeLast();
						//	sb.append( '}' );
						//
						//	this->location->setHint( sb.asString() );
						//}
						delete filter;
						delete files;
					}
					break;
				}
			}
			break;
		}
	}
};

class LocationContainer : public openocl::ui::OpaqueContainer
{
public:
	LocationContainer( const String& name, unsigned int width, FocusManager& fm )
	: OpaqueContainer( Color::DIALOG ), label( "Location" ), location( name )
	{
		this->location.setPreferredWidth( width );

		///this->setLayout( new TableLayout( 2 ) );
		this->setEventMask( Event::MOUSE_EVENT );
		//this->add( label );
		this->add( location );

		//fm.addComponent( *this );
		fm.addComponent( this->location );
	}
	
	~LocationContainer()
	{
		//this->remove( label );
		this->remove( location );
	}

	Label         label;
	LocationField location;
};

};};};

LocationBar::LocationBar( const String& name, unsigned int width )
: Control( "openocl::controls::LocationBar" )
{
	this->base = new openocl::controls::locationbarcontrol::LocationContainer( name, width, this->getFocusManager() );
	this->base->addEventListener( *this );
	this->setControlComponent( *this->base );
}

LocationBar::~LocationBar()
{
	this->getFocusManager().purge();
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
LocationBar::handleActionEvent( const ActionEvent& actionEvent )
{
	switch ( actionEvent.getType() )
	{
	case Event::FOCUS_EVENT:
		{
			//const FocusEvent& fe = dynamic_cast<const FocusEvent&>( actionEvent );
			//FocusEvent fe2( *this, 0, fe.getFocusEventType() ); 
			//this->fireEvent( actionEvent );
		}
		break;
	}

	return false;
}

void
LocationBar::setLabel( const openocl::base::String& aLabel )
{
	dynamic_cast<openocl::controls::locationbarcontrol::LocationContainer*>( this->base )->label.setText( aLabel );
}

void
LocationBar::setLocation( const openocl::base::String& location )
{
	dynamic_cast<openocl::controls::locationbarcontrol::LocationContainer*>( this->base )->location.setData( location );
}

const String&
LocationBar::getLocation() const
{
	return dynamic_cast<openocl::controls::locationbarcontrol::LocationContainer*>( this->base )->location.getData();
}

//------------------------------------------------------------------------------
//	private methods
//------------------------------------------------------------------------------



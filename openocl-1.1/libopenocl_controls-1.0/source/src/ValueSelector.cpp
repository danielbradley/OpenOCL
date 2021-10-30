/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/controls/ValueSelector.h"
#include "openocl/base/FormattedString.h"
#include <openocl/base/Iterator.h>
#include <openocl/base/Math.h>
#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/ChangeEvent.h>
#include <openocl/base/event/Event.h>
#include <openocl/base/event/KeyEvent.h>
#include <openocl/data/TableData.h>
#include <openocl/data/Tuple.h>
#include <openocl/imaging/Color.h>
#include <openocl/ui/Button.h>
#include <openocl/ui/Container.h>
#include <openocl/ui/GraphicsContext.h>
#include <openocl/ui/Icon.h>
#include <openocl/ui/IconGroup.h>
#include <openocl/ui/Label.h>
#include <openocl/ui/NumberField.h>
#include <openocl/ui/OffscreenImageManager.h>
#include <openocl/ui/OpaqueContainer.h>
#include <openocl/ui/Panel.h>
#include <openocl/ui/Region.h>
#include <openocl/ui/TableLayout.h>
#include <openocl/ui/TextField.h>
#include <openocl/ui/SidewaysLayout.h>
#include <openocl/ui/Visual.h>
#include <openocl/util/Sequence.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::controls;
using namespace openocl::data;
using namespace openocl::imaging;
using namespace openocl::ui;
using namespace openocl::util;

namespace openocl {
	namespace controls {
		namespace valueselectorcontrol {


class Base : public openocl::ui::Container
{
public:
	Base( TableData& data, const String& fieldName )
	: Container(), data( data ), fieldName( fieldName ), field( fieldName ), value( "0" ), down( "Down", "DOWN", "<" ), up( "Up", "UP", ">" )
	{
		this->setEdge( 5 );
		this->setMargin( 0 );
		this->setBorder( 0 );
		this->setPadding( 0 );

		this->fieldNr = -1;
		this->tupleNr = 0;
		this->valueNr = 0;

		this->field.setPreferredWidth( 50 );
		this->value.setPreferredWidth( 100 );
		this->value.setEditable( false );
	
		this->setLayout( new TableLayout( 4 ) );
		this->add( this->field );
		this->add( this->value );
		this->add( this->down );
		this->add( this->up );
	}
	
	~Base()
	{
		this->remove( this->field );
		this->remove( this->value );
		this->remove( this->down );
		this->remove( this->up );
	}

	//---------------------------------------------------------------------
	//	virtual public methods (EventListener)
	//---------------------------------------------------------------------
	void deliverEvent( const Event& anEvent )
	{
		flags type = anEvent.getType();
		if ( Event::ACTION_EVENT == type )
		{
			if ( -1 == this->fieldNr )
			{
				return;
				this->init();
			}

			const String& command = dynamic_cast<const ActionEvent&>( anEvent ).getCommand();
			bool fire_event = false;

			if ( command.startsWith( "DOWN." ) )
			{
				//fprintf( stderr, "ValueSelector::deliverEvent: DOWN\n" );

				Tuple*  current_tuple        = this->data.tupleAt( this->tupleNr );
				String* current_value_string = current_tuple->valueAt( this->fieldNr );
				{
					double current_value = current_value_string->getValue();
				
					for ( int i=this->tupleNr-1; i >= 0; i-- )
					{
						Tuple*  tuple        = this->data.tupleAt( i );
						String* value_string = tuple->valueAt( this->fieldNr );
						double value = value_string->getValue();
					
						if ( value < current_value )
						{
							this->tupleNr = i;
							this->valueNr--;
							this->value.setData( *value_string );
							//fprintf( stderr, "ValueSelector::deliverEvent: setting value %i: %s\n", this->valueNr, value_string->getChars() );
							i = -1;
							fire_event = true;
						}
					
						delete value_string;
						delete tuple;
					}
				}
				delete current_tuple;
				delete current_value_string;
			}
			else if ( command.startsWith( "UP." ) )
			{
				//fprintf( stderr, "ValueSelector::deliverEvent: UP\n" );
				
				Tuple*  current_tuple = this->data.tupleAt( this->tupleNr );
				String* current_value_string = current_tuple->valueAt( this->fieldNr );
				double current_value = current_value_string->getValue();
				
				int max = this->data.getNrOfRows();
				for ( int i=this->tupleNr+1; i < max; i++ )
				{
					Tuple*  tuple = this->data.tupleAt( i );
					String* value_string = tuple->valueAt( this->fieldNr );
					double value = value_string->getValue();
					
					if ( value > current_value )
					{
						this->tupleNr = i;
						this->valueNr++;
						this->value.setData( *value_string );
						//fprintf( stderr, "ValueSelector::deliverEvent: setting value %i: %s\n", this->valueNr, value_string->getChars() );
						i = max;
						fire_event = true;
					}
					
					delete value_string;
					delete tuple;
				}

				delete current_tuple;
				delete current_value_string;
			}
			
			if ( fire_event )
			{
				FormattedString num( "%i", this->valueNr );
				StringBuffer sb;
				sb.append( this->fieldName );
				sb.append( ".openocl::controls::ValueSelector.CLICKED" );
				
				ActionEvent ae( *this, 0, sb.asString(), num );
				fireEvent( ae );
			}
		}
	}

	//---------------------------------------------------------------------
	//	virtual public methods (Component)
	//---------------------------------------------------------------------

	//Dimensions getPreferredLayoutDimensions( const GraphicsContext& gc, unsigned int width, unsigned int height ) const
	//{
	//	unsigned int = this-

	//---------------------------------------------------------------------
	//	private methods
	//---------------------------------------------------------------------

	void init()
	{
		Tuple* row = this->data.tupleAt( this->tupleNr );
		if ( row ) {
			unsigned int max = row->getNrOfFields();
			for ( unsigned int i=0; i < max; i++ )
			{
				String* fieldName = row->fieldName( i );
				if ( fieldName->equals( this->fieldName ) )
				{
					this->fieldNr = i;
					i = max;
				}
				delete fieldName;
			}
			
			String* value = row->valueAt( this->fieldNr );
			this->value.setData( *value );
			delete value;
		}
		delete row;
	}
	
	void refresh()
	{
		//fprintf( stderr, "ValueSelector::refresh()\n" );

		if ( -1 == this->fieldNr )
		{
			this->init();
		}
		
		if ( -1 != this->fieldNr )
		{
			Tuple* tuple = this->data.tupleAt( this->tupleNr );
			{
				String* value = tuple->valueAt( this->fieldNr );
				this->value.setData( *value );
				delete value;
			}
			delete tuple;
		}
	}
	
	TableData& data;

	String    fieldName;
	Label     field;
	TextField value;
	Button    down;
	Button    up;

	int fieldNr;
	int tupleNr;
	int valueNr;
};

};};};

ValueSelector::ValueSelector( TableData& data, const String& fieldName )
: Control( "openocl::controls::ValueSelector" )
{
	this->base = new openocl::controls::valueselectorcontrol::Base( data, fieldName );
	this->setControlComponent( *this->base );
	this->base->addEventListener( *this );
}

ValueSelector::~ValueSelector()
{
	this->base->removeEventListener( *this );
	delete this->base;
}

//--------------------------------------------
//	public virtual methods (EventListener)
//--------------------------------------------

bool
ValueSelector::handleActionEvent( const ActionEvent& anActionEvent )
{
	//const String& command = anActionEvent.getCommand();
	//const String& data    = anActionEvent.getData();
	//fprintf( stderr, "ValueSelector::deliverEvent: command=\"%s\" data=\"%s\"\n", command.getChars(), data.getChars() );

	return false;
}

void
ValueSelector::refresh()
{
	//ChangeEvent ce( *this, 0, ChangeEvent::REDRAW_ALL );
	dynamic_cast<openocl::controls::valueselectorcontrol::Base*>( this->base )->refresh();
	//this->base->forwardEvent( ce );
}


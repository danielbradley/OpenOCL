/*
 *  Copyright (C) 1997-2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/ui/Selector.h"

#include <openocl/base/String.h>
#include <openocl/base/StringBuffer.h>
#include <openocl/base/event/ActionEvent.h>
#include <openocl/base/event/Event.h>

#include <cstdio>

using namespace openocl::base;
using namespace openocl::base::event;
using namespace openocl::ui;

Selector::Selector( const String& name ) : Textual( name )
{
	this->action = new String();
	this->label = new String();
	this->data = new String();
}

Selector::Selector( const String& name, const String& action ) : Textual( name )
{
	this->action = new String( action );
	this->label = new String();
	this->data = new String();
}

Selector::Selector( const String& name, const String& action, const String& label ) : Textual( name )
{
	this->action = new String( action );
	this->label = new String( label );
	this->data = new String();
}

Selector::~Selector()
{
	delete this->action;
	delete this->label;
	delete this->data;
}

void
Selector::fireSelectorEvent( const String& className )
{
#ifdef DEBUG_OPENOCL_UI_SELECTOR
	fprintf( stderr, "Selector::fireSelectorEvent: %s %s\n",
			className.getChars(), this->data->getChars() );
#endif

	StringBuffer sb;
	sb.append( *this->action );
	sb.append( '.' );
	sb.append( className );
	ActionEvent ae( *this, 0, sb.asString(), *this->data );
	this->Textual::fireEvent( ae );
}

void
Selector::setAction( const openocl::base::String& action )
{
	delete this->action;
	this->action = new String( action );
}

void
Selector::setData( const openocl::base::String& data )
{
	delete this->data;
	this->data = new String( data );
}

void
Selector::setLabel( const openocl::base::String& label )
{
	delete this->label;
	this->label = new String( label );
}

const String&
Selector::getAction() const
{
	return *this->action;
}

const String&
Selector::getData() const
{
	//fprintf( stderr, "%s.Selector::getData( %s )\n", this->getName().getChars(), this->data->getChars() ); 
	return *this->data;
}

const String&
Selector::getLabel() const
{
	return *this->label;
}

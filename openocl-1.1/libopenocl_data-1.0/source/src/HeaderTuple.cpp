/*
 *  Copyright (C) 2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include "openocl/data/HeaderTuple.h"

#include <openocl/base/FormattedString.h>
#include <openocl/base/String.h>
#include <openocl/util/Sequence.h>

#include <cstdlib>

using namespace openocl::base;
using namespace openocl::data;
using namespace openocl::util;

HeaderTuple::HeaderTuple()
{
	this->fields = new Sequence();
}

HeaderTuple::HeaderTuple( const HeaderTuple& aTuple )
{
	this->fields = aTuple.fields->copy();
}

HeaderTuple::~HeaderTuple()
{
	delete this->fields;
}

void
HeaderTuple::setFieldValue( unsigned int fieldNr, const String& value )
{
	String* value_copy = new String( value );
	try
	{
		this->fields->add( fieldNr, value_copy );
	}
	catch (NoSuchElementException* ex)
	{
		delete ex;
		delete value_copy;
		FormattedString default_field_name( "%i", fieldNr - 1 );
		this->setFieldValue( fieldNr - 1, default_field_name );
	}
}

const String&
HeaderTuple::getFieldName( unsigned int fieldNr ) const
{
	return this->getValueAt( fieldNr );
}

const String&
HeaderTuple::getValueAt( unsigned int fieldNr ) const
{
	return dynamic_cast<const String&>( this->fields->elementAt( fieldNr ) );
} 

String*
HeaderTuple::fieldName( unsigned int fieldNr ) const
{
	return this->valueAt( fieldNr );
}

String*
HeaderTuple::valueAt( unsigned int fieldNr ) const
{
	return new String( dynamic_cast<const String&>( this->fields->elementAt( fieldNr ) ) );
} 

unsigned int
HeaderTuple::getNrOfFields() const
{
	return this->fields->getLength();
}

Tuple*
HeaderTuple::copy() const
{
	return new HeaderTuple( *this );
}

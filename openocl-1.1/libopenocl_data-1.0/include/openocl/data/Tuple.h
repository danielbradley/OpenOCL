/*
 *  Copyright (C) 2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_DATA_TUPLE_H
#define OPENOCL_DATA_TUPLE_H

#include <openocl/base.h>
#include <openocl/base/Interface.h>
#include <openocl/data.h>

namespace openocl {
	namespace data {

class Tuple : public openocl::base::Interface
{
public:
	virtual ~Tuple();

	//	public virtual methods
	virtual void setFieldValue( unsigned int fieldNr, const openocl::base::String& value ) = 0;

	//	public constant virtual methods
	virtual const openocl::base::String& getFieldName( unsigned int fieldNr ) const = 0;
	virtual const openocl::base::String& getValueAt( unsigned int fieldNr ) const = 0;

	//	public constant virtual methods (allocating)
	virtual openocl::base::String* fieldName( unsigned int fieldNr ) const = 0;
	virtual openocl::base::String* valueAt( unsigned int fieldNr ) const = 0;

	virtual unsigned int getNrOfFields() const = 0;
	virtual Tuple* copy() const = 0;
};

};};

#endif

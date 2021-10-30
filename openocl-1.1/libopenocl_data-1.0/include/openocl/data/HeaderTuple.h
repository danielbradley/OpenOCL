/*
 *  Copyright (C) 2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_DATA_HEADERTUPLE_H
#define OPENOCL_DATA_HEADERTUPLE_H

#include <openocl/base.h>
#include <openocl/base/Object.h>
#include <openocl/data.h>
#include <openocl/data/Tuple.h>
#include <openocl/util.h>

namespace openocl {
	namespace data {

class HeaderTuple : public openocl::base::Object, public Tuple
{
public:
	HeaderTuple();
	explicit HeaderTuple( const HeaderTuple& aTuple );
	virtual ~HeaderTuple();

	//	public virtual methods (Tuple)
	virtual void setFieldValue( unsigned int fieldNr, const openocl::base::String& value );

	//	public constant virtual methods
	virtual const openocl::base::String& getFieldName( unsigned int fieldNr ) const;
	virtual const openocl::base::String& getValueAt( unsigned int fieldNr ) const;

	//	public constant virtual methods (Tuple)
	virtual openocl::base::String* fieldName( unsigned int fieldNr ) const;
	virtual openocl::base::String* valueAt( unsigned int fieldNr ) const;

	virtual unsigned int getNrOfFields() const;

	virtual Tuple* copy() const;

private:
	openocl::util::Sequence* fields;
};

};};

#endif

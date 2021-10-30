/*
 *  Copyright (C) 2005 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_DATA_TABLEDATA_H
#define OPENOCL_DATA_TABLEDATA_H

#include <openocl/base.h>
#include <openocl/base/Interface.h>
#include <openocl/data.h>

namespace openocl {
	namespace data {

class TableData : public openocl::base::Interface
{
public:
	virtual ~TableData();

	//	public constant virtual methods
	virtual TableData*   query( const openocl::base::String& queryString ) const = 0;
	virtual HeaderTuple* headerTuple() const = 0;
	virtual Tuple*       tupleAt( unsigned int tupleNr ) const = 0;

	virtual unsigned int getNrOfRows() const = 0;
	virtual unsigned int getNrOfCols() const = 0;
};

};};

#endif

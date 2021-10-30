/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MPI_MPIEXCEPTION_H
#define OPENOCL_MPI_MPIEXCEPTION_H

#include "openocl/mpi.h"

#include <openocl/base/Exception.h>
#include <openocl/base.h>

namespace openocl {
	namespace mpi {

class MPIException : public openocl::base::Exception
{
public:
	MPIException();
	MPIException( const openocl::base::String* aMessage );
	MPIException( const openocl::base::String& aMessage );
	MPIException( const openocl::base::StringBuffer& aMessage );

	MPIException( const char* aMessage );
	MPIException( int errno );
	virtual ~MPIException();
  
	virtual const openocl::base::String& getType() const;
};

};};

#endif

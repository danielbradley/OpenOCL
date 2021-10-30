/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <openocl/mpi/MPIException.h>

#include <openocl/base/Error.h>
#include <openocl/base/String.h>

using namespace openocl::base;
using namespace openocl::mpi;

static const String EXCEPTION_TYPE( "MPIException" );

MPIException::MPIException()
{
}

MPIException::MPIException( const String* aMessage )
: Exception( aMessage )
{
}

MPIException::MPIException( const String& aMessage )
: Exception( aMessage )
{
}

MPIException::MPIException( const StringBuffer& aMessage )
: Exception( aMessage )
{
}

MPIException::MPIException( const char* aMessage ) : Exception( aMessage )
{
}

MPIException::MPIException( int errno )
{
  this->message = Error::retrieveMessage( errno );
}

MPIException::~MPIException()
{
}

const String&
MPIException::getType() const
{
  return EXCEPTION_TYPE;
}

//String*
//String::toString() const
//{
//  StringBuffer* sb = new StringBuffer();

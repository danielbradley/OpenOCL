/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#include <openocl/base/psa/CharString.h>
#include <openocl/mpi/MPIApplication.h>


#include <mpi.h>

using namespace openocl::base::psa;
using namespace openocl::mpi;

MPIApplication::MPIApplication( int* argcPtr, char*** argvPtr )
{
	MPI_Init( argcPtr, argvPtr );
	MPI_Comm_rank( MPI_COMM_WORLD, &this->rank );
	MPI_Comm_size( MPI_COMM_WORLD, &this->totalNrOfProcesses );
}

MPIApplication::~MPIApplication()
{
	MPI_Finalize();
}

void
MPIApplication::sendText( int destination, const char* message, int aTag )
throw (MPIException*)
{
	int len = CharString_getLength( message );
	int error = 0;
	if ( (error = MPI_Send( (void*) message, len, MPI_CHAR, destination, aTag, MPI_COMM_WORLD )) )
	{
		throw new MPIException( error );
	}
}

int
MPIApplication::receiveText( char* buffer, int size, int sender, int tag )
throw (MPIException*)
{
	int error = 0;
	int count = 0;
	MPI_Status status;
	if ( (error = MPI_Recv( (void*) buffer, size, MPI_CHAR, sender, tag, MPI_COMM_WORLD, &status )) )
	{
		throw new MPIException( error );
	}
	if ( (error = MPI_Get_count( &status, MPI_CHAR, &count )) )
	{
		throw new MPIException( error );
	}
	return count;
}

int
MPIApplication::getRank() const
{
	return this->rank;
}

int
MPIApplication::getTotalNrOfProcesses() const
{
	return this->totalNrOfProcesses;
}

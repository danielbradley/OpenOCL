/*
 *  Copyright (C) 1997-2004 Daniel Robert Bradley. All rights reserved.
 *
 *  This software is redistributable under the terms of the GNU LGPL
 *  (Lesser General Public License).
 *
 *  I.S.Labs is a registered trademark of Daniel Robert Bradley
 */

#ifndef OPENOCL_MPI_MPIAPPLICATION_H
#define OPENOCL_MPI_MPIAPPLICATION_H

#include "openocl/mpi/MPIException.h"

#include <openocl/base/Object.h>

namespace openocl {
	namespace mpi {

class MPIApplication : public openocl::base::Object
{
public:
	virtual ~MPIApplication();
	virtual bool run() = 0;
	
	void sendText( int destination, const char* message, int aTag )
		throw (MPIException*);
	int receiveText( char* buffer, int size, int sender, int aTag )
		throw (MPIException*);
	
	int getRank() const;
	int getTotalNrOfProcesses() const;

protected:
	MPIApplication( int* argcPtr, char*** argvPtr );

private:
	int rank;
	int totalNrOfProcesses;
};

};};

#endif

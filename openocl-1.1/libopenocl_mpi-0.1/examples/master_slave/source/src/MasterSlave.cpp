#include "MasterSlave.h"

#include <openocl/base/Runtime.h>
#include <cstdio>

#define WORKTAG 1

using namespace openocl::base;


MasterSlave::MasterSlave( int* argc, char*** argv ) : MPIApplication( argc, argv )
{}

MasterSlave::~MasterSlave()
{}

bool
MasterSlave::run()
{
	if ( 0 == this->getRank() )
	{
		this->runMaster();
	} else {
		this->runSlave();
	}

	return true;
}

void
MasterSlave::runMaster()
{
	this->sendText( 1, "Hello world", WORKTAG );
}

void
MasterSlave::runSlave()
{
	char* buffer = (char*) Runtime::calloc( 512, sizeof( char ) );
	if ( this->receiveText( buffer, 512, 0, WORKTAG ) )
	{
		fprintf( stdout, "Messsage: %s", buffer );
	}
	Runtime::free( buffer );
}


int main( int argc, char** argv )
{
	MasterSlave master_slave( &argc, &argv );
	return !master_slave.run();
}

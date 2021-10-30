#include <openocl/mpi/MPIApplication.h>

class MasterSlave : public openocl::mpi::MPIApplication
{
public:
	MasterSlave( int* arc, char*** argv );
	~MasterSlave();
	virtual bool run();

protected:
	void runMaster();
	void runSlave();
};

#include "App.h"
#include <openocl/base/String.h>
#include <openocl/base/Testing.h>

using namespace openocl::base;

int    ARGC;
char** ARGV;

bool test()
{
	App app( ARGC, ARGV );
	app.start();
	return true;
}

int main( int argc, char** argv )
{
	int result = 0;

	String str( "--test" );
	if ( 1 < argc && str.equals( argv[1] ) )
	{
		ARGC = argc;
		ARGV = argv;
		result = !Testing::runTest( "Text Viewer", test );
	}
	else
	{
		App app( argc, argv );
		app.start();
	}

	return result;
}

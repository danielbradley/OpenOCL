#include "windows.h"

extern int main( int, char**);

int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	int argc = 1;
	char** argv = new char*[2];
	
	argv[0] = "analyse";
	argv[1] = NULL;
	
	return main( argc, argv );
}

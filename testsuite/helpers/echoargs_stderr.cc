// Output command line args on stderr

#include <stdio.h>

int main ( int argc, char *argv[] )
{
    for ( int i=1; i < argc; i++ )
    {
	fprintf ( stderr, "stderr #%d: %s\n", i, argv[i] );
    }
   
    return 0;
}

// Output command line args on stdout

#include <stdio.h>

int main ( int argc, char *argv[] )
{
    for ( int i=1; i < argc; i++ )
    {
	printf ( "stdout #%d: %s\n", i, argv[i] );
    }
   
    return 0;
}

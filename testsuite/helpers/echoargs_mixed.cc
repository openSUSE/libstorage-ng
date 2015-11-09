// Output command line args alternatingly on stdout and stderr.
// This is useful for testing separating or mixing stdout and stderr in SystemCmd.

#include <stdio.h>

int main ( int argc, char *argv[] )
{
    int stdout_count = 0;
    int stderr_count = 0;
    
    for ( int i=1; i < argc; i++ )
    {
	if ( i%2 == 1 )
	{
	    fprintf ( stdout, "line #%d: stdout #%d: %s\n", i, ++stdout_count, argv[i] );
	}
	else
	{
	    fprintf ( stderr, "line #%d: stderr #%d: %s\n", i, ++stderr_count, argv[i] );
	}
    }

    return 0;
}

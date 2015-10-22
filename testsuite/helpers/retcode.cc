// Return the return code given as command line argument as exit code.

#include <stdlib.h>

int main ( int argc, char *argv[] )
{
    int retCode = 0;
    
    if ( argc > 1 )
    {
	retCode = atoi( argv[1] );
    }
   
    return retCode;
}

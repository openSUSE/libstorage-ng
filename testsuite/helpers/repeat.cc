// Repeat command line arg 2 on stdout arg 1 number of times

#include <cstdio>
#include <cstdlib>

int
main (int argc, char* argv[])
{
    for (int i = 0; i < atoi(argv[1]); ++i)
    {
	printf("%s\n", argv[2]);
    }

    return EXIT_SUCCESS;
}

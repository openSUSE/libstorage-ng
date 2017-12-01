

#include <iostream>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Exception.h"


using namespace std;
using namespace storage;


void
doit(const string& text)
{
    try
    {
	unsigned long long value = humanstring_to_byte(text, false);

	cout << value << " " << get_suffix(0, false) << '\n';
	cout << byte_to_humanstring(value, false, 4, true) << '\n';
    }
    catch (const ParseException& e)
    {
	cerr << "failed to parse value\n";
    }
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "humanstring value\n";
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    if (argc != 2)
        usage();

    try
    {
	doit(argv[1]);
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << '\n';
	exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

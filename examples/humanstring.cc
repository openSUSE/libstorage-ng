

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

	cout << value << " " << get_suffix(0, false) << endl;
	cout << byte_to_humanstring(value, false, 4, true) << endl;
    }
    catch (const ParseException& e)
    {
	cerr << "failed to parse value" << endl;
    }
}


void usage() __attribute__ ((__noreturn__));

void
usage()
{
    cerr << "humanstring value" << endl;
    exit(EXIT_FAILURE);
}


int
main(int argc, char **argv)
{
    if (argc != 2)
        usage();

    doit(argv[1]);

    exit(EXIT_SUCCESS);
}

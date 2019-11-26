
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmddmsetuptable(SystemInfo& system_info)
{
    try
    {
	const CmdDmsetupTable& cmddmsetuptable = system_info.getCmdDmsetupTable();
	cout << "CmdDmsetupTable success" << endl;
	cout << cmddmsetuptable << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdDmsetupTable failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_cmddmsetuptable(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmddmsetuptable(SystemInfo& systeminfo)
{
    try
    {
	const CmdDmsetupTable& cmddmsetuptable = systeminfo.getCmdDmsetupTable();
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

    SystemInfo systeminfo;

    test_cmddmsetuptable(systeminfo);
}

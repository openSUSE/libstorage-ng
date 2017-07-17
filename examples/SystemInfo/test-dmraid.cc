
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_dmraid(SystemInfo& system_info)
{
    try
    {
	const CmdDmraid& cmd_dmraid = system_info.getCmdDmraid();
	cout << "Dmraid success" << endl;
	cout << cmd_dmraid << endl;
    }
    catch (const exception& e)
    {
	cerr << "Dmraid failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_dmraid(system_info);
}

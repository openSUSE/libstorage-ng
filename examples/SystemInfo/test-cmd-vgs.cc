
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_vgs(SystemInfo& system_info)
{
    try
    {
	const CmdVgs& cmd_vgs = system_info.getCmdVgs();
	cout << "CmdVgs success" << endl;
	cout << cmd_vgs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdVgs failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_vgs(system_info);
}

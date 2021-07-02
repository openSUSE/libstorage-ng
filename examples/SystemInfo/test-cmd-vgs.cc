
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_vgs(SystemInfo::Impl& system_info)
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

    SystemInfo::Impl system_info;

    test_vgs(system_info);
}

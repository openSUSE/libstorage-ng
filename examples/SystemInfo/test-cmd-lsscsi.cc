
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_lsscsi(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdLsscsi& cmd_lsscsi = system_info.getCmdLsscsi();
	cout << "CmdLsscsi success" << endl;
	cout << cmd_lsscsi << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdLsscsi failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_lsscsi(system_info);
}

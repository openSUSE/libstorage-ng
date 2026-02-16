
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_lsattr(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdLsattr& cmd_lsattr = system_info.getCmdLsattr("/dev/system/btrfs", "/btrfs",
							      "/var/lib/mariadb");
	cout << "CmdLsattr success" << endl;
	cout << cmd_lsattr << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdLsattr failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_lsattr(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_lsattr(SystemInfo& systeminfo)
{
    try
    {
	const CmdLsattr& cmd_lsattr = systeminfo.getCmdLsattr("/dev/system/btrfs", "/btrfs",
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

    SystemInfo systeminfo;

    test_lsattr(systeminfo);
}

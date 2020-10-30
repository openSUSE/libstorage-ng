
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmd_btrfs_qgroup_show(SystemInfo& system_info, const string& path)
{
    try
    {
	const CmdBtrfsQgroupShow& cmd_btrfs_qgroup_show =
	    system_info.getCmdBtrfsQgroupShow("/dev/sdc1", path);
	cout << "CmdBtrfsQgroupShow success" << endl;
	cout << cmd_btrfs_qgroup_show << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsQgroupShow failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_cmd_btrfs_qgroup_show(system_info, "/btrfs");
}

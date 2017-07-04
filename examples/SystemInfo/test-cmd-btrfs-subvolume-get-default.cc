
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmd_btrfs_subvolume_get_default(SystemInfo& system_info, const string& path)
{
    try
    {
	const CmdBtrfsSubvolumeGetDefault& cmd_btrfs_subvolume_get_default =
	    system_info.getCmdBtrfsSubvolumeGetDefault("/dev/system/btrfs", path);
	cout << "CmdBtrfsSubvolumeGetDefault success" << endl;
	cout << cmd_btrfs_subvolume_get_default << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsSubvolumeGetDefault failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_cmd_btrfs_subvolume_get_default(system_info, "/btrfs");
}

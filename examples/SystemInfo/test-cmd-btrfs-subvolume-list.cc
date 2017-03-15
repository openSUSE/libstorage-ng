
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmd_btrfs_subvolume_list(SystemInfo& systeminfo, const string& path)
{
    try
    {
	const CmdBtrfsSubvolumeList& cmd_btrfs_subvolume_list =
	    systeminfo.getCmdBtrfsSubvolumeList("/dev/system/btrfs", path);
	cout << "CmdBtrfsSubvolumeList success" << endl;
	cout << cmd_btrfs_subvolume_list << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsSubvolumeList failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_cmd_btrfs_subvolume_list(systeminfo, "/btrfs");
}

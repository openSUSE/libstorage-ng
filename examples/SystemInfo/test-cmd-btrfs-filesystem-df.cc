
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmd_btrfs_filesystem_df(SystemInfo& system_info, const string& path)
{
    try
    {
	const CmdBtrfsFilesystemDf& cmd_btrfs_filesystem_df =
	    system_info.getCmdBtrfsFilesystemDf("/dev/sdc1", path);
	cout << "CmdBtrfsFilesystemDf success" << endl;
	cout << cmd_btrfs_filesystem_df << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsFilesystemDf failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_cmd_btrfs_filesystem_df(system_info, "/test1");
}

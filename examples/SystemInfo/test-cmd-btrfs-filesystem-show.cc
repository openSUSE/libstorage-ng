
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_cmd_btrfs_filesystem_show(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdBtrfsFilesystemShow& cmdbtrfsfilesystemshow = system_info.getCmdBtrfsFilesystemShow();
	cout << "CmdBtrfsFilesystemShow success" << endl;
	cout << cmdbtrfsfilesystemshow << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsFilesystemShow failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_cmd_btrfs_filesystem_show(system_info);
}

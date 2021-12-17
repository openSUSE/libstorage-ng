
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_blockdev(SystemInfo::Impl& system_info, const string& path)
{
    try
    {
	const CmdBlockdev& cmd_blockdev = system_info.getCmdBlockdev(path);
	cout << "CmdBlockdev success for '" << path << "'" << endl;
	cout << cmd_blockdev << endl;
	cout << "get_size:" << cmd_blockdev.get_size() << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBlockdev failed for '" << path << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_blockdev(system_info, "/dev/sda");
}

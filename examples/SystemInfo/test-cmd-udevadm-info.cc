
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_udevadm_info(SystemInfo& system_info, const string& file)
{
    try
    {
	const CmdUdevadmInfo& cmd_udevadm_info = system_info.getCmdUdevadmInfo(file);
	cout << "CmdUdevadmInfo success for '" << file << "'" << endl;
	cout << cmd_udevadm_info << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdUdevmapInfo failed for '" << file << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_udevadm_info(system_info, "/dev/sda1");
}

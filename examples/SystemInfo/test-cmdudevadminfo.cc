
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_udevadminfo(SystemInfo& system_info, const string& file)
{
    try
    {
	const CmdUdevadmInfo& cmdudevadminfo = system_info.getCmdUdevadmInfo(file);
	cout << "CmdUdevadmInfo success for '" << file << "'" << endl;
	cout << cmdudevadminfo << endl;
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

    test_udevadminfo(system_info, "/dev/sda1");
}

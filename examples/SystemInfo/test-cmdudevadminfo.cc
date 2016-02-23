
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_udevadminfo(SystemInfo& systeminfo, const string& file)
{
    try
    {
	const CmdUdevadmInfo& cmdudevadminfo = systeminfo.getCmdUdevadmInfo(file);
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

    SystemInfo systeminfo;

    test_udevadminfo(systeminfo, "/dev/sda1");
}

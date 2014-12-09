
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_udevmap(SystemInfo& systeminfo, const string& path)
{
    try
    {
	const UdevMap& udevmap = systeminfo.getUdevMap(path);
	cout << "UdevMap success for '" << path << "'" << endl;
	cout << udevmap << endl;
    }
    catch (const exception& e)
    {
	cerr << "UdevMap failed for '" << path << "'" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_udevmap(systeminfo, "/dev/disk/by-id");
}

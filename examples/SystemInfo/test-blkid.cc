
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_blkid(SystemInfo& systeminfo)
{
    try
    {
	const Blkid& blkid = systeminfo.getBlkid();
	cout << "Blkid success" << endl;
	cout << blkid << endl;
    }
    catch (const exception& e)
    {
	cerr << "Blkid failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_blkid(systeminfo);
}

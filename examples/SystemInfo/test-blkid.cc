
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
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_blkid(systeminfo);
}

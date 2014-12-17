
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_lsscsi(SystemInfo& systeminfo)
{
    try
    {
	const Lsscsi& lsscsi = systeminfo.getLsscsi();
	cout << "Lsscsi success" << endl;
	cout << lsscsi << endl;
    }
    catch (const exception& e)
    {
	cerr << "Lsscsi failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_lsscsi(systeminfo);
}

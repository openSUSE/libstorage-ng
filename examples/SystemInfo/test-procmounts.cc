
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procmounts(SystemInfo& systeminfo)
{
    try
    {
	const ProcMounts& procmounts = systeminfo.getProcMounts();
	cout << "ProcMounts success" << endl;
	cout << procmounts << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcMounts failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_procmounts(systeminfo);
}

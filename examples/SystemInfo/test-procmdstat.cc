
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procmdstat(SystemInfo& systeminfo)
{
    try
    {
	const ProcMdstat& procmdstat = systeminfo.getProcMdstat();
	cout << "ProcMdstat success" << endl;
	cout << procmdstat << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcMdstat failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_procmdstat(systeminfo);
}

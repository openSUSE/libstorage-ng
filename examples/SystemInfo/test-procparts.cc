
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procparts(SystemInfo& systeminfo)
{
    try
    {
	const ProcParts& procparts = systeminfo.getProcParts();
	cout << "ProcParts success" << endl;
	cout << procparts << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcParts failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_procparts(systeminfo);
}

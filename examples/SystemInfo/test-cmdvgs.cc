
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_vgs(SystemInfo& systeminfo)
{
    try
    {
	const CmdVgs& cmdvgs = systeminfo.getCmdVgs();
	cout << "CmdVgs success" << endl;
	cout << cmdvgs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdVgs failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_vgs(systeminfo);
}

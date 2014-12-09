
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_mdadmexamine(SystemInfo& systeminfo, const list<string>& devices)
{
    try
    {
	const MdadmExamine& mdadmexamine = systeminfo.getMdadmExamine(devices);
	cout << "MdadmExamine success" << endl;
	cout << mdadmexamine << endl;
    }
    catch (const exception& e)
    {
	cerr << "MdadmExamine failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_mdadmexamine(systeminfo, { "/dev/sda", "/dev/sdb" });
}

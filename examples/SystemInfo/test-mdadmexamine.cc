
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdadmexamine(SystemInfo& systeminfo, const vector<string>& devices)
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
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_mdadmexamine(systeminfo, { "/dev/sda", "/dev/sdb" });
}

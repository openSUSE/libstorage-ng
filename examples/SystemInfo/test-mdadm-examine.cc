
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdadmexamine(SystemInfo& system_info, const vector<string>& devices)
{
    try
    {
	const MdadmExamine& mdadmexamine = system_info.getMdadmExamine(devices);
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

    SystemInfo system_info;

    test_mdadmexamine(system_info, { "/dev/sda", "/dev/sdb" });
}

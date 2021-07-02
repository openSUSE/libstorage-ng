
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_mdadm_examine(SystemInfo::Impl& system_info, const vector<string>& devices)
{
    try
    {
	const MdadmExamine& mdadm_examine = system_info.getMdadmExamine(devices);
	cout << "MdadmExamine success" << endl;
	cout << mdadm_examine << endl;
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

    SystemInfo::Impl system_info;

    test_mdadm_examine(system_info, { "/dev/sda", "/dev/sdb" });
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdadm_detail(SystemInfo& system_info, const string& device)
{
    try
    {
	const MdadmDetail& mdadm_detail = system_info.getMdadmDetail(device);
	cout << "MdadmDetail success" << endl;
	cout << mdadm_detail << endl;
    }
    catch (const exception& e)
    {
	cerr << "MdadmDetail failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_mdadm_detail(system_info, "/dev/md127");
}

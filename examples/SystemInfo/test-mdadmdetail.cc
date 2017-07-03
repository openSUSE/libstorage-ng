
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdadmdetail(SystemInfo& system_info, const string& device)
{
    try
    {
	const MdadmDetail& mdadmdetail = system_info.getMdadmDetail(device);
	cout << "MdadmDetail success" << endl;
	cout << mdadmdetail << endl;
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

    test_mdadmdetail(system_info, "/dev/md127");
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdadmdetail(SystemInfo& systeminfo, const string& device)
{
    try
    {
	const MdadmDetail& mdadmdetail = systeminfo.getMdadmDetail(device);
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

    SystemInfo systeminfo;

    test_mdadmdetail(systeminfo, "/dev/md127");
}


#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_multipath(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdMultipath& cmd_multipath = system_info.getCmdMultipath();
	cout << "Multipath success" << endl;
	cout << cmd_multipath << endl;
    }
    catch (const exception& e)
    {
	cerr << "Multipath failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_multipath(system_info);
}

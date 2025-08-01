
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_mdadm_detail(SystemInfo::Impl& system_info, const string& device)
{
    try
    {
	const CmdMdadmDetail& cmd_mdadm_detail = system_info.getCmdMdadmDetail(device);
	cout << "CmdMdadmDetail success" << endl;
	cout << cmd_mdadm_detail << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdMdadmDetail failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_mdadm_detail(system_info, "/dev/md127");
}

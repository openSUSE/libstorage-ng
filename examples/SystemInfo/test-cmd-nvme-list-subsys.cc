
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_nvme_list_subsys(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdNvmeListSubsys& cmd_nvme_list_subsys = system_info.getCmdNvmeListSubsys();
	cout << "CmdNvmeListSubsys success" << endl;
	cout << cmd_nvme_list_subsys << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdNvmeListSubsys failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_nvme_list_subsys(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_nvme_list(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdNvmeList& cmd_nvme_list = system_info.getCmdNvmeList();
	cout << "CmdNvmeList success" << endl;
	cout << cmd_nvme_list << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdNvmeList failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_nvme_list(system_info);
}

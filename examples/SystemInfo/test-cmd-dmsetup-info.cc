
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_cmd_dmsetup_info(SystemInfo::Impl& system_info)
{
    try
    {
	const CmdDmsetupInfo& cmd_dmsetup_info = system_info.getCmdDmsetupInfo();
	cout << "CmdDmsetupInfo success" << endl;
	cout << cmd_dmsetup_info << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdDmsetupInfo failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_cmd_dmsetup_info(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmddmsetupinfo(SystemInfo& system_info)
{
    try
    {
	const CmdDmsetupInfo& cmddmsetupinfo = system_info.getCmdDmsetupInfo();
	cout << "CmdDmsetupInfo success" << endl;
	cout << cmddmsetupinfo << endl;
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

    SystemInfo system_info;

    test_cmddmsetupinfo(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_proc_mdstat(SystemInfo& system_info)
{
    try
    {
	const ProcMdstat& proc_mdstat = system_info.getProcMdstat();
	cout << "ProcMdstat success" << endl;
	cout << proc_mdstat << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcMdstat failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_proc_mdstat(system_info);
}

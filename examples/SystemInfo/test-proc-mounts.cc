
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_proc_mounts(SystemInfo& system_info)
{
    try
    {
	const ProcMounts& proc_mounts = system_info.getProcMounts();
	cout << "ProcMounts success" << endl;
	cout << proc_mounts << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcMounts failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_proc_mounts(system_info);
}

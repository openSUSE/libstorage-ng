
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procmounts(SystemInfo& system_info)
{
    try
    {
	const ProcMounts& procmounts = system_info.getProcMounts();
	cout << "ProcMounts success" << endl;
	cout << procmounts << endl;
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

    test_procmounts(system_info);
}

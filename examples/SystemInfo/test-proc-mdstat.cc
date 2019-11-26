
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procmdstat(SystemInfo& system_info)
{
    try
    {
	const ProcMdstat& procmdstat = system_info.getProcMdstat();
	cout << "ProcMdstat success" << endl;
	cout << procmdstat << endl;
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

    test_procmdstat(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procparts(SystemInfo& system_info)
{
    try
    {
	const ProcParts& procparts = system_info.getProcParts();
	cout << "ProcParts success" << endl;
	cout << procparts << endl;
    }
    catch (const exception& e)
    {
	cerr << "ProcParts failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_procparts(system_info);
}

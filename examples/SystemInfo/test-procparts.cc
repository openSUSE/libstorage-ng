
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_procparts(SystemInfo& systeminfo)
{
    try
    {
	const ProcParts& procparts = systeminfo.getProcParts();
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

    SystemInfo systeminfo;

    test_procparts(systeminfo);
}

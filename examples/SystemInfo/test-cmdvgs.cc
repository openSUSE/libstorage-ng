
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_vgs(SystemInfo& systeminfo)
{
    try
    {
	const CmdVgs& cmdvgs = systeminfo.getCmdVgs();
	cout << "CmdVgs success" << endl;
	cout << cmdvgs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdVgs failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_vgs(systeminfo);
}

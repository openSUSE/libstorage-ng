
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_pvs(SystemInfo& systeminfo)
{
    try
    {
	const CmdPvs& cmd_pvs = systeminfo.getCmdPvs();
	cout << "CmdPvs success" << endl;
	cout << cmd_pvs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdPvs failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_pvs(systeminfo);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_lvs(SystemInfo& systeminfo)
{
    try
    {
	const CmdLvs& cmd_lvs = systeminfo.getCmdLvs();
	cout << "CmdLvs success" << endl;
	cout << cmd_lvs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdLvs failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_lvs(systeminfo);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_vgdisplay(SystemInfo& systeminfo, const string& name)
{
    try
    {
	const CmdVgdisplay& cmdvgdisplay = systeminfo.getCmdVgdisplay(name);
	cout << "CmdVgdisplay success" << endl;
	cout << cmdvgdisplay << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdVgdisplay failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_vgdisplay(systeminfo, "system");
}

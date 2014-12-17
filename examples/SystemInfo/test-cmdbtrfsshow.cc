
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_cmdbtrfsshow(SystemInfo& systeminfo)
{
    try
    {
	const CmdBtrfsShow& cmdbtrfsshow = systeminfo.getCmdBtrfsShow();
	cout << "CmdBtrfsShow success" << endl;
	cout << cmdbtrfsshow << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdBtrfsShow failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_cmdbtrfsshow(systeminfo);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_cmddmsetupinfo(SystemInfo& systeminfo)
{
    try
    {
	const CmdDmsetupInfo& cmddmsetupinfo = systeminfo.getCmdDmsetupInfo();
	cout << "CmdDmsetupInfo success" << endl;
	cout << cmddmsetupinfo << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdDmsetupInfo failed" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_cmddmsetupinfo(systeminfo);
}

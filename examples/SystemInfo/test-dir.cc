
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_dir(SystemInfo& systeminfo, const string& path)
{
    try
    {
	const Dir& dir = systeminfo.getDir(path);
	cout << "Dir success for '" << path << "'" << endl;
	cout << dir << endl;
    }
    catch (const exception& e)
    {
	cerr << "Dir failed for '" << path << "'" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_dir(systeminfo, "/sys/block");
}

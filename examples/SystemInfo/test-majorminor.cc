
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage_bgl;


void
test_majorminor(SystemInfo& systeminfo, const string& device)
{
    try
    {
	const MajorMinor& majorminor = systeminfo.getMajorMinor(device);
	cout << "MajorMinor success for '" << device << "'" << endl;
	cout << majorminor << endl;
    }
    catch (const exception& e)
    {
	cerr << "MajorMinor failed for '" << device << "' with '" << e.what() << "'" << endl;
    }
}


int
main()
{
    createLogger("/var/log/YaST2", "libstorage");

    SystemInfo systeminfo;

    test_majorminor(systeminfo, "/dev/sda");

    test_majorminor(systeminfo, "/dev/disk/by-label/BOOT");

    test_majorminor(systeminfo, "/dev/does-not-exist");

    test_majorminor(systeminfo, "/dev/char/5:0");
}

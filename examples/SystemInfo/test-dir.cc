
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_dir(SystemInfo& system_info, const string& path)
{
    try
    {
	const Dir& dir = system_info.getDir(path);
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
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_dir(system_info, "/sys/block");
}

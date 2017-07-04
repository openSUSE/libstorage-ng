
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_lsscsi(SystemInfo& system_info)
{
    try
    {
	const Lsscsi& lsscsi = system_info.getLsscsi();
	cout << "Lsscsi success" << endl;
	cout << lsscsi << endl;
    }
    catch (const exception& e)
    {
	cerr << "Lsscsi failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_lsscsi(system_info);
}

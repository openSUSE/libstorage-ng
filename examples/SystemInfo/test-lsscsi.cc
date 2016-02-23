
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_lsscsi(SystemInfo& systeminfo)
{
    try
    {
	const Lsscsi& lsscsi = systeminfo.getLsscsi();
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

    SystemInfo systeminfo;

    test_lsscsi(systeminfo);
}

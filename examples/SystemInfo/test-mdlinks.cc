
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdlinks(SystemInfo& systeminfo)
{
    try
    {
	const MdLinks& mdlinks = systeminfo.getMdLinks();
	cout << "MdLinks success" << endl;
	cout << mdlinks << endl;
    }
    catch (const exception& e)
    {
	cerr << "Mdlinks failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_mdlinks(systeminfo);
}

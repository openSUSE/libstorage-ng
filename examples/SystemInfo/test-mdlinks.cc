
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_mdlinks(SystemInfo& system_info)
{
    try
    {
	const MdLinks& mdlinks = system_info.getMdLinks();
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

    SystemInfo system_info;

    test_mdlinks(system_info);
}


#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_md_links(SystemInfo& system_info)
{
    try
    {
	const MdLinks& md_links = system_info.getMdLinks();
	cout << "MdLinks success" << endl;
	cout << md_links << endl;
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

    test_md_links(system_info);
}

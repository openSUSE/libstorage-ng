
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_dasdview(SystemInfo::Impl& system_info, const string& device)
{
    try
    {
	const Dasdview& dasdview = system_info.getDasdview(device);
	cout << "Dasdview success for '" << device << "'" << endl;
	cout << dasdview << endl;
    }
    catch (const exception& e)
    {
	cerr << "Dasdview failed for '" << device << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_dasdview(system_info, "/dev/dasda");
}


#include <iostream>

#include <storage/SystemInfo/CmdResize2fs.h>
#include <storage/Utils/Logger.h>

using namespace std;
using namespace storage;


void
test_cmd_resize2fs(const string& device)
{
    try
    {
	const CmdResize2fs cmd_resize2fs(device);
	cout << "CmdResize2fs success for '" << device << "'" << endl;
	cout << cmd_resize2fs << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdResize2fs failed for '" << device << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    test_cmd_resize2fs("/dev/sdc1");
}

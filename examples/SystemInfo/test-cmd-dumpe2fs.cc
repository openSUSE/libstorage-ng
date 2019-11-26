
#include <iostream>

#include <storage/SystemInfo/CmdDumpe2fs.h>
#include <storage/Utils/Logger.h>

using namespace std;
using namespace storage;


void
test_cmd_dumpe2fs(const string& device)
{
    try
    {
	const CmdDumpe2fs cmd_dumpe2fs(device);
	cout << "CmdDumpe2fs success for '" << device << "'" << endl;
	cout << cmd_dumpe2fs << endl;
    }
    catch (const exception& e)
    {
	cerr << "Dumpe2fs failed for '" << device << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    test_cmd_dumpe2fs("/dev/sdc1");
}

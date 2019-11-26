
#include <iostream>

#include <storage/SystemInfo/CmdNtfsresize.h>
#include <storage/Utils/Logger.h>

using namespace std;
using namespace storage;


void
test_cmd_ntfsresize(const string& device)
{
    try
    {
	const CmdNtfsresize cmd_ntfsresize(device);
	cout << "CmdNtfsresize success for '" << device << "'" << endl;
	cout << cmd_ntfsresize << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdNtfsresize failed for '" << device << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    test_cmd_ntfsresize("/dev/sdd1");
}

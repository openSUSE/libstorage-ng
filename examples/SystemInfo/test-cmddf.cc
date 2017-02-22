
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_df(SystemInfo& systeminfo, const string& path)
{
    try
    {
	const CmdDf& cmd_df = systeminfo.getCmdDf(path);
	cout << "CmdDf success for '" << path << "'" << endl;
	cout << cmd_df << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdDf failed for '" << path << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo systeminfo;

    test_df(systeminfo, "/home/arvin");
}

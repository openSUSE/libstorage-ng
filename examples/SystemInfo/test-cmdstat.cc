
#include <iostream>

#include <storage/SystemInfo/SystemInfo.h>

using namespace std;
using namespace storage;


void
test_stat(SystemInfo& system_info, const string& path)
{
    try
    {
	const CmdStat& cmd_stat = system_info.getCmdStat(path);
	cout << "CmdStat success for '" << path << "'" << endl;
	cout << cmd_stat << endl;
	cout << "is_blk:" << cmd_stat.is_blk() << endl;
	cout << "is_dir:" << cmd_stat.is_dir() << endl;
	cout << "is_reg:" << cmd_stat.is_reg() << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdStat failed for '" << path << "'" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo system_info;

    test_stat(system_info, "/dev/sda");
}

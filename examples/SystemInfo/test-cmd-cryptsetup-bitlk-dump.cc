
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_cmd_cryptsetup_bitlk_dump(SystemInfo::Impl& system_info, const string& name)
{
    try
    {
	const CmdCryptsetupBitlkDump& cmd_cryptsetup_bitlk_dump = system_info.getCmdCryptsetupBitlkDump(name);
	cout << "CmdCryptsetupBitlkDump success" << endl;
	cout << cmd_cryptsetup_bitlk_dump << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdCryptsetupBitlkDump failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_cmd_cryptsetup_bitlk_dump(system_info, "/dev/sdc1");
}

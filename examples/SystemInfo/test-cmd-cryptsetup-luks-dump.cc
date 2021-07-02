
#include <iostream>

#include <storage/SystemInfo/SystemInfoImpl.h>

using namespace std;
using namespace storage;


void
test_cmd_cryptsetup_luks_dump(SystemInfo::Impl& system_info, const string& name)
{
    try
    {
	const CmdCryptsetupLuksDump& cmd_cryptsetup_luks_dump = system_info.getCmdCryptsetupLuksDump(name);
	cout << "CmdCryptsetupLuksDump success" << endl;
	cout << cmd_cryptsetup_luks_dump << endl;
    }
    catch (const exception& e)
    {
	cerr << "CmdCryptsetupLuksDump failed" << endl;
    }
}


int
main()
{
    set_logger(get_logfile_logger());

    SystemInfo::Impl system_info;

    test_cmd_cryptsetup_luks_dump(system_info, "/dev/sdc1");
}

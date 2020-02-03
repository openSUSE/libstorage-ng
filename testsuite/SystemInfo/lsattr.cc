
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "storage/Utils/Exception.h"
#include "storage/SystemInfo/CmdLsattr.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(LSATTR_BIN " -d (device:/dev/system/btrfs path:/var/lib/mariadb)", input);

    CmdLsattr cmd_lsattr(
	CmdLsattr::key_t("/dev/system/btrfs", "/var/lib/mariadb"), "/btrfs", "/var/lib/mariadb"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_lsattr;

    string lhs = parsed.str();
    string rhs;

    if (!output.empty())
	rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"----------------C-- /btrfs/var/lib/mariadb"
    };

    vector<string> output = {
	"mountpoint:/btrfs path:/var/lib/mariadb nocow:true"
    };

    check(input, output);
}

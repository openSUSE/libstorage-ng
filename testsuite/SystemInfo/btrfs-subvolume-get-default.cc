
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include <iostream>

#include "storage/Utils/Exception.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(BTRFSBIN " subvolume get-default (device:/dev/system/btrfs)", input);

    CmdBtrfsSubvolumeGetDefault cmd_btrfs_subvolume_get_default(
	CmdBtrfsSubvolumeGetDefault::key_t("/dev/system/btrfs"), "/btrfs"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_btrfs_subvolume_get_default;

    string lhs = parsed.str();
    string rhs;

    if (!output.empty())
	rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"ID 5 (FS_TREE)"
    };

    vector<string> output = {
	"id:5"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"ID 14261 gen 22049 top level 5 path test"
    };

    vector<string> output = {
        "id:14261"
    };

    check(input, output);
}


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
    Mockup::set_command(BTRFSBIN " subvolume list -a -p (device:/dev/system/btrfs)", input);

    CmdBtrfsSubvolumeList cmd_btrfs_subvolume_list(
	CmdBtrfsSubvolumeList::key_t("/dev/system/btrfs"), "/btrfs"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_btrfs_subvolume_list;

    string lhs = parsed.str();
    string rhs;

    if (!output.empty())
	rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse_good)
{
    vector<string> input = {
	"ID 1996 gen 21472 parent 5 top level 5 path home",
	"ID 2794 gen 22066 parent 5 top level 5 path .snapshots"
    };

    vector<string> output = {
	"id:1996 parent-id:5 path:home",
	"id:2794 parent-id:5 path:.snapshots"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_empty)
{
    check({}, {});
}

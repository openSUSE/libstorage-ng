
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

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
    Mockup::set_command(BTRFS_BIN " subvolume show (device:/dev/system/btrfs)", input);

    CmdBtrfsSubvolumeShow cmd_btrfs_subvolume_show(
	CmdBtrfsSubvolumeShow::key_t("/dev/system/btrfs"), "/btrfs"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_btrfs_subvolume_show;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse_good)
{
    vector<string> input = {
	"/",
	"        Name:                   <FS_TREE>",
	"        UUID:                   d6b02b4f-368c-4c49-b749-60ccbafeaa9a",
	"        Parent UUID:            -",
	"        Received UUID:          -",
	"        Creation time:          2020-05-07 15:00:46 +0200",
	"        Subvolume ID:           5",
	"        Generation:             10",
	"        Gen at creation:        0",
	"        Parent ID:              0",
	"        Top level ID:           0",
	"        Flags:                  -",
	"        Snapshot(s):",
	"                                1a"
    };

    vector<string> output = {
	"uuid:d6b02b4f-368c-4c49-b749-60ccbafeaa9a"
    };

    check(input, output);
}

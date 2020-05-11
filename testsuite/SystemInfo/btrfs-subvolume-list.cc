
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
    Mockup::set_command(BTRFS_BIN " subvolume list -a -puq (device:/dev/system/btrfs)", input);

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
	"ID 256 gen 10 parent 5 top level 5 parent_uuid d6b02b4f-368c-4c49-b749-60ccbafeaa9a uuid a3dc5067-ec7e-f046-8538-e768583d1f4e path 1a",
	"ID 258 gen 10 parent 259 top level 259 parent_uuid -                                    uuid 19e6acf1-5fbe-8345-be44-8cd6685d39a2 path <FS_TREE>/2b/2a",
	"ID 259 gen 10 parent 5 top level 5 parent_uuid 19e6acf1-5fbe-8345-be44-8cd6685d39a2 uuid 2c58057e-640b-dc48-92d5-9d1c35185d7a path 2b"
    };

    vector<string> output = {
	"id:256 parent-id:5 path:1a uuid:a3dc5067-ec7e-f046-8538-e768583d1f4e parent-uuid:d6b02b4f-368c-4c49-b749-60ccbafeaa9a",
	"id:258 parent-id:259 path:2b/2a uuid:19e6acf1-5fbe-8345-be44-8cd6685d39a2",
	"id:259 parent-id:5 path:2b uuid:2c58057e-640b-dc48-92d5-9d1c35185d7a parent-uuid:19e6acf1-5fbe-8345-be44-8cd6685d39a2"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_empty)
{
    check({}, {});
}

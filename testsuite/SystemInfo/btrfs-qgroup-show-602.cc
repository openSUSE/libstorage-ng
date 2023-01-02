
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <numeric>
#include <boost/test/unit_test.hpp>

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
    Mockup::set_command(BTRFS_BIN " --version", RemoteCommand({ "btrfs-progs v6.0.2" }, {}, 0));
    Mockup::set_command(BTRFS_BIN " qgroup show -rep --raw (device:/dev/system/btrfs)", input);

    CmdBtrfsQgroupShow cmd_btrfs_qgroup_show(
	CmdBtrfsQgroupShow::key_t("/dev/system/btrfs"), "/btrfs"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_btrfs_qgroup_show;

    string lhs = parsed.str();
    string rhs = accumulate(output.begin(), output.end(), ""s,
			    [](auto a, auto b) { return a + b + "\n"; });

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse)
{
    set_logger(get_stdout_logger());

    vector<string> input = {
	"Qgroupid    Referenced    Exclusive  Max referenced  Max exclusive Parent     Path ",
	"--------    ----------    ---------  --------------  ------------- ------     ---- ",
	"0/5              16384        16384            none           none -          <toplevel>",
	"0/256            16384        16384            none           none 1/0        a",
	"0/257            16384        16384            none           none -          b",
	"1/0              32768        32768            none     2147483648 -          <0 member qgroups>",
	"2/0                  0            0      1073741824           none -          <0 member qgroups>"
    };

    vector<string> output = {
	"id:0/5 referenced:16384 exclusive:16384",
	"id:0/256 referenced:16384 exclusive:16384 parents:1/0",
	"id:0/257 referenced:16384 exclusive:16384",
	"id:1/0 referenced:32768 exclusive:32768 exclusive-limit:2147483648",
	"id:2/0 referenced:0 exclusive:0 referenced-limit:1073741824"
    };

    check(input, output);
}

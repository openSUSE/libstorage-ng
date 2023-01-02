
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
    Mockup::set_command(BTRFS_BIN " --version", RemoteCommand({ "btrfs-progs v6.0" }, {}, 0));
    Mockup::set_command(BTRFS_BIN " filesystem df (device:/dev/system/btrfs)", input);

    CmdBtrfsFilesystemDf cmd_btrfs_filesystem_get_default(
	CmdBtrfsFilesystemDf::key_t("/dev/system/btrfs"), "/btrfs"
    );

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_btrfs_filesystem_get_default;

    string lhs = parsed.str();
    string rhs;

    if (!output.empty())
	rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"Data, single: total=8.00MiB, used=64.00KiB",
	"System, DUP: total=8.00MiB, used=16.00KiB",
	"Metadata, DUP: total=102.25MiB, used=112.00KiB",
	"GlobalReserve, single: total=16.00MiB, used=0.00B"
    };

    vector<string> output = {
	"metadata-raid-level:DUP data-raid-level:SINGLE"
    };

    check(input, output);
}


/*
 * In mixed mode metadata and data are identical and reported together.
 */
BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"System, single: total=4.00MiB, used=4.00KiB",
	"Data+Metadata, single: total=840.00MiB, used=220.00KiB",
	"GlobalReserve, single: total=16.00MiB, used=0.00B"
    };

    vector<string> output = {
	"metadata-raid-level:SINGLE data-raid-level:SINGLE"
    };

    check(input, output);
}


/*
 * During a balance job to convert the RAID level several RAID levels can be
 * reported. Since this is only an interim state it is not handle in all its
 * beauty. Instead just the last reported RAID level is used.
 */
BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"Data, RAID10: total=4.00GiB, used=3.35GiB",
	"Data, RAID5: total=3.00GiB, used=509.71MiB",
	"System, RAID10: total=64.00MiB, used=16.00KiB",
	"Metadata, RAID10: total=512.00MiB, used=247.55MiB",
	"Metadata, RAID5: total=288.00MiB, used=5.56MiB",
	"GlobalReserve, single: total=23.91MiB, used=0.00B"
    };

    vector<string> output = {
	"metadata-raid-level:RAID5 data-raid-level:RAID5"
    };

    check(input, output);
}

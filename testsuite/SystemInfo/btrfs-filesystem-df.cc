
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
    Mockup::set_command(BTRFSBIN " filesystem df (device:/dev/system/btrfs)", input);

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

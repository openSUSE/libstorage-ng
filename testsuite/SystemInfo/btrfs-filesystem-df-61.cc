
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
    Mockup::set_command(BTRFS_BIN " --version", RemoteCommand({ "btrfs-progs v6.1" }, {}, 0));
    Mockup::set_command(BTRFS_BIN " --format json filesystem df (device:/dev/system/btrfs)", input);

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
	R"({)",
	R"(  "__header": {)",
	R"(    "version": "1")",
	R"(  },)",
	R"(  "filesystem-df": [)",
	R"(    {)",
	R"(      "bg-type": "Data",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "8388608",)",
	R"(      "used": "0")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "System",)",
	R"(      "bg-profile": "DUP",)",
	R"(      "total": "8388608",)",
	R"(      "used": "16384")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "Metadata",)",
	R"(      "bg-profile": "DUP",)",
	R"(      "total": "268435456",)",
	R"(      "used": "196608")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "GlobalReserve",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "3670016",)",
	R"(      "used": "0")",
	R"(    })",
	R"(  ])",
	R"(})"
    };

    vector<string> output = {
	"metadata-raid-level:DUP data-raid-level:SINGLE"
    };

    check(input, output);
}


/*
 * In mixed mode metadata and data are identical and reported together.
 *
 * 'barrel create btrfs --size 1g /dev/sdd --mkfs-options --mixed --path /test'
 */
BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	R"({)",
	R"(  "__header": {)",
	R"(    "version": "1")",
	R"(  },)",
	R"(  "filesystem-df": [)",
	R"(    {)",
	R"(      "bg-type": "System",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "4194304",)",
	R"(      "used": "4096")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "Data+Metadata",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "8388608",)",
	R"(      "used": "32768")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "GlobalReserve",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "917504",)",
	R"(      "used": "0")",
	R"(    })",
	R"(  ])",
	R"(})"
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
 *
 * 'barrel create btrfs --pool-name "HDDs (512 B)" --size 50g --devices 4 --profiles raid10 --path /test'
 * 'btrfs balance start -dconvert=raid5 -mconvert=raid5 /test'
 */
BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	R"({)",
	R"(  "__header": {)",
	R"(    "version": "1")",
	R"(  },)",
	R"(  "filesystem-df": [)",
	R"(    {)",
	R"(      "bg-type": "Data",)",
	R"(      "bg-profile": "RAID10",)",
	R"(      "total": "4294967296",)",
	R"(      "used": "4197752832")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "Data",)",
	R"(      "bg-profile": "RAID5",)",
	R"(      "total": "6442450944",)",
	R"(      "used": "97214464")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "System",)",
	R"(      "bg-profile": "RAID10",)",
	R"(      "total": "33554432",)",
	R"(      "used": "0")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "System",)",
	R"(      "bg-profile": "RAID5",)",
	R"(      "total": "50331648",)",
	R"(      "used": "16384")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "Metadata",)",
	R"(      "bg-profile": "RAID10",)",
	R"(      "total": "1073741824",)",
	R"(      "used": "4521984")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "Metadata",)",
	R"(      "bg-profile": "RAID5",)",
	R"(      "total": "2214592512",)",
	R"(      "used": "262144")",
	R"(    },)",
	R"(    {)",
	R"(      "bg-type": "GlobalReserve",)",
	R"(      "bg-profile": "single",)",
	R"(      "total": "4702208",)",
	R"(      "used": "0")",
	R"(    })",
	R"(  ])",
	R"(})"
    };

    vector<string> output = {
	"metadata-raid-level:RAID5 data-raid-level:RAID5"
    };

    check(input, output);
}

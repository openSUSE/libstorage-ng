
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
	"{",
	"  \"__header\": {",
	"    \"version\": \"1\"",
	"  },",
	"  \"filesystem-df\": [",
	"    {",
	"      \"bg-type\": \"Data\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"8388608\",",
	"      \"used\": \"0\"",
	"    },",
	"    {",
	"      \"bg-type\": \"System\",",
	"      \"bg-profile\": \"DUP\",",
	"      \"total\": \"8388608\",",
	"      \"used\": \"16384\"",
	"    },",
	"    {",
	"      \"bg-type\": \"Metadata\",",
	"      \"bg-profile\": \"DUP\",",
	"      \"total\": \"268435456\",",
	"      \"used\": \"196608\"",
	"    },",
	"    {",
	"      \"bg-type\": \"GlobalReserve\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"3670016\",",
	"      \"used\": \"0\"",
	"    }",
	"  ]",
	"}"
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
	"{",
	"  \"__header\": {",
	"    \"version\": \"1\"",
	"  },",
	"  \"filesystem-df\": [",
	"    {",
	"      \"bg-type\": \"System\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"4194304\",",
	"      \"used\": \"4096\"",
	"    },",
	"    {",
	"      \"bg-type\": \"Data+Metadata\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"8388608\",",
	"      \"used\": \"32768\"",
	"    },",
	"    {",
	"      \"bg-type\": \"GlobalReserve\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"917504\",",
	"      \"used\": \"0\"",
	"    }",
	"  ]",
	"}"
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
	"{",
	"  \"__header\": {",
	"    \"version\": \"1\"",
	"  },",
	"  \"filesystem-df\": [",
	"    {",
	"      \"bg-type\": \"Data\",",
	"      \"bg-profile\": \"RAID10\",",
	"      \"total\": \"4294967296\",",
	"      \"used\": \"4197752832\"",
	"    },",
	"    {",
	"      \"bg-type\": \"Data\",",
	"      \"bg-profile\": \"RAID5\",",
	"      \"total\": \"6442450944\",",
	"      \"used\": \"97214464\"",
	"    },",
	"    {",
	"      \"bg-type\": \"System\",",
	"      \"bg-profile\": \"RAID10\",",
	"      \"total\": \"33554432\",",
	"      \"used\": \"0\"",
	"    },",
	"    {",
	"      \"bg-type\": \"System\",",
	"      \"bg-profile\": \"RAID5\",",
	"      \"total\": \"50331648\",",
	"      \"used\": \"16384\"",
	"    },",
	"    {",
	"      \"bg-type\": \"Metadata\",",
	"      \"bg-profile\": \"RAID10\",",
	"      \"total\": \"1073741824\",",
	"      \"used\": \"4521984\"",
	"    },",
	"    {",
	"      \"bg-type\": \"Metadata\",",
	"      \"bg-profile\": \"RAID5\",",
	"      \"total\": \"2214592512\",",
	"      \"used\": \"262144\"",
	"    },",
	"    {",
	"      \"bg-type\": \"GlobalReserve\",",
	"      \"bg-profile\": \"single\",",
	"      \"total\": \"4702208\",",
	"      \"used\": \"0\"",
	"    }",
	"  ]",
	"}"
    };

    vector<string> output = {
	"metadata-raid-level:RAID5 data-raid-level:RAID5"
    };

    check(input, output);
}

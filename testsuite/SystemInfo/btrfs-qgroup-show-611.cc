
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
    Mockup::set_command(BTRFS_BIN " --version", RemoteCommand({ "btrfs-progs v6.1.1" }, {}, 0));
    Mockup::set_command(BTRFS_BIN " --format json qgroup show -rep --raw (device:/dev/system/btrfs)", input);

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
	"{",
	"  \"__header\": {",
	"    \"version\": \"1\"",
	"  },",
	"  \"qgroup-show\": [",
	"    {",
	"      \"qgroupid\": \"0/5\",",
	"      \"referenced\": \"16384\",",
	"      \"max_referenced\": \"none\",",
	"      \"exclusive\": \"16384\",",
	"      \"max_exclusive\": \"none\",",
	"      \"path\": \"\",",
	"      \"parents\": [",
	"      ],",
	"      \"children\": [",
	"      ]",
	"    },",
	"    {",
	"      \"qgroupid\": \"0/256\",",
	"      \"referenced\": \"16384\",",
	"      \"max_referenced\": \"none\",",
	"      \"exclusive\": \"16384\",",
	"      \"max_exclusive\": \"none\",",
	"      \"path\": \"a\",",
	"      \"parents\": [",
	"        \"1/0\"",
	"      ],",
	"      \"children\": [",
	"      ]",
	"    },",
	"    {",
	"      \"qgroupid\": \"0/257\",",
	"      \"referenced\": \"16384\",",
	"      \"max_referenced\": \"none\",",
	"      \"exclusive\": \"16384\",",
	"      \"max_exclusive\": \"none\",",
	"      \"path\": \"b\",",
	"      \"parents\": [",
	"      ],",
	"      \"children\": [",
	"      ]",
	"    },",
	"    {",
	"      \"qgroupid\": \"1/0\",",
	"      \"referenced\": \"32768\",",
	"      \"max_referenced\": \"none\",",
	"      \"exclusive\": \"32768\",",
	"      \"max_exclusive\": \"2147483648\",",
	"      \"path\": \"\",",
	"      \"parents\": [",
	"      ],",
	"      \"children\": [",
	"        \"0/256\",",
	"        \"0/257\"",
	"      ]",
	"    },",
	"    {",
	"      \"qgroupid\": \"2/0\",",
	"      \"referenced\": \"0\",",
	"      \"max_referenced\": \"1073741824\",",
	"      \"exclusive\": \"0\",",
	"      \"max_exclusive\": \"none\",",
	"      \"path\": \"\",",
	"      \"parents\": [",
	"      ],",
	"      \"children\": [",
	"      ]",
	"    }",
	"  ]",
	"}"
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

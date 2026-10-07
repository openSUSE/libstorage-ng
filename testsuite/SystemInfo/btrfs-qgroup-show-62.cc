
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
    Mockup::set_command(BTRFS_BIN " --version", RemoteCommand({ "btrfs-progs v6.2" }, {}, 0));
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
	R"({)",
	R"(  "__header": {)",
	R"(    "version": "1")",
	R"(  },)",
	R"(  "qgroup-show": [)",
	R"(    {)",
	R"(      "qgroupid": "0/5",)",
	R"(      "referenced": "16384",)",
	R"(      "max_referenced": "none",)",
	R"(      "exclusive": "16384",)",
	R"(      "max_exclusive": "none",)",
	R"(      "path": "",)",
	R"(      "parents": [)",
	R"(      ],)",
	R"(      "children": [)",
	R"(      ])",
	R"(    },)",
	R"(    {)",
	R"(      "qgroupid": "0/256",)",
	R"(      "referenced": "16384",)",
	R"(      "max_referenced": "none",)",
	R"(      "exclusive": "16384",)",
	R"(      "max_exclusive": "none",)",
	R"(      "path": "a",)",
	R"(      "parents": [)",
	R"(        "1/0")",
	R"(      ],)",
	R"(      "children": [)",
	R"(      ])",
	R"(    },)",
	R"(    {)",
	R"(      "qgroupid": "0/257",)",
	R"(      "referenced": "16384",)",
	R"(      "max_referenced": "none",)",
	R"(      "exclusive": "16384",)",
	R"(      "max_exclusive": "none",)",
	R"(      "path": "b",)",
	R"(      "parents": [)",
	R"(      ],)",
	R"(      "children": [)",
	R"(      ])",
	R"(    },)",
	R"(    {)",
	R"(      "qgroupid": "1/0",)",
	R"(      "referenced": "32768",)",
	R"(      "max_referenced": "none",)",
	R"(      "exclusive": "32768",)",
	R"(      "max_exclusive": "2147483648",)",
	R"(      "path": "",)",
	R"(      "parents": [)",
	R"(      ],)",
	R"(      "children": [)",
	R"(        "0/256",)",
	R"(        "0/257")",
	R"(      ])",
	R"(    },)",
	R"(    {)",
	R"(      "qgroupid": "2/0",)",
	R"(      "referenced": "0",)",
	R"(      "max_referenced": "1073741824",)",
	R"(      "exclusive": "0",)",
	R"(      "max_exclusive": "none",)",
	R"(      "path": "",)",
	R"(      "parents": [)",
	R"(      ],)",
	R"(      "children": [)",
	R"(      ])",
	R"(    })",
	R"(  ])",
	R"(})"
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

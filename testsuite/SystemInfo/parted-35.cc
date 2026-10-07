
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& stdout, const vector<string>& stderr,
      const vector<string>& result)
{
    setenv("LIBSTORAGE_OS_FLAVOUR", "suse", 1);

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --version", RemoteCommand({ "parted (GNU parted) 3.5" }, {}, 0));
    Mockup::set_command({ PARTED_BIN, "--script", "--json", device, "unit", "s", "print" },
			RemoteCommand(stdout, stderr, 0));

    Udevadm udevadm;

    CmdParted cmd_parted(udevadm, device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_parted;

    string lhs = parsed.str();
    string rhs = boost::join(result, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& device, const vector<string>& stdout, const vector<string>& result)
{
    check(device, stdout, vector<string>(), result);
}


void
check_exception(const string& device, const vector<string>& input)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --version", RemoteCommand({ "parted (GNU parted) 3.5" }, {}, 0));
    Mockup::set_command({ PARTED_BIN, "--script", "--json", device, "unit", "s", "print" }, input);

    Udevadm udevadm;

    BOOST_CHECK_THROW({ CmdParted cmd_parted(udevadm, device); }, Exception);
}


BOOST_AUTO_TEST_CASE(parse_msdos)
{
    vector<string> input = {
	R"({)",
	R"(   "disk": {)",
	R"(      "path": "/dev/sdc",)",
	R"(      "size": "625142448s",)",
	R"(      "model": "ATA WDC WD3200BEKT-6",)",
	R"(      "transport": "scsi",)",
	R"(      "logical-sector-size": 512,)",
	R"(      "physical-sector-size": 4096,)",
	R"(      "label": "msdos",)",
	R"(      "max-partitions": 4,)",
	R"(      "partitions": [)",
	R"(         {)",
	R"(            "number": 1,)",
	R"(            "start": "2048s",)",
	R"(            "end": "20973567s",)",
	R"(            "size": "20971520s",)",
	R"(            "type": "primary",)",
	R"(            "type-id": "0x83")",
	R"(         },{)",
	R"(            "number": 2,)",
	R"(            "start": "20973568s",)",
	R"(            "end": "83888127s",)",
	R"(            "size": "62914560s",)",
	R"(            "type": "extended",)",
	R"(            "type-id": "0x0f",)",
	R"(            "flags": [)",
	R"(                "lba")",
	R"(            ])",
	R"(         },{)",
	R"(            "number": 5,)",
	R"(            "start": "20975616s",)",
	R"(            "end": "62918655s",)",
	R"(            "size": "41943040s",)",
	R"(            "type": "logical",)",
	R"(            "type-id": "0xfd",)",
	R"(            "flags": [)",
	R"(                "raid")",
	R"(            ])",
	R"(         })",
	R"(      ])",
	R"(   })",
	R"(})"
    };

    vector<string> output = {
	"device:/dev/sdc label:MS-DOS region:[0, 625142448, 512 B] primary-slots:4",
	"number:1 region:[2048, 20971520, 512 B] type:primary id:0x83",
	"number:2 region:[20973568, 62914560, 512 B] type:extended id:0x0f",
	"number:5 region:[20975616, 41943040, 512 B] type:logical id:0xfd",
    };

    check("/dev/sdc", input, output);
}


BOOST_AUTO_TEST_CASE(parse_gpt)
{
    vector<string> input = {
	R"({)",
	R"(   "disk": {)",
	R"(      "path": "/dev/sdc",)",
	R"(      "size": "625142448s",)",
	R"(      "model": "ATA WDC WD3200BEKT-6",)",
	R"(      "transport": "scsi",)",
	R"(      "logical-sector-size": 512,)",
	R"(      "physical-sector-size": 4096,)",
	R"(      "label": "gpt",)",
	R"(      "max-partitions": 128,)",
	R"(      "partitions": [)",
	R"(         {)",
	R"(            "number": 1,)",
	R"(            "start": "2048s",)",
	R"(            "end": "20973567s",)",
	R"(            "size": "20971520s",)",
	R"(            "type": "primary",)",
	R"(            "type-uuid": "0fc63daf-8483-4772-8e79-3d69d8477de4")",
	R"(         },{)",
	R"(            "number": 2,)",
	R"(            "start": "20973568s",)",
	R"(            "end": "62916607s",)",
	R"(            "size": "41943040s",)",
	R"(            "type": "primary",)",
	R"(            "type-uuid": "a19d880f-05fc-4d3b-a006-743f0f84911e",)",
	R"(            "flags": [)",
	R"(                "raid")",
	R"(            ])",
	R"(         })",
	R"(      ])",
	R"(   })",
	R"(})"
    };

    vector<string> output = {
	"device:/dev/sdc label:GPT region:[0, 625142448, 512 B] primary-slots:128",
	"number:1 region:[2048, 20971520, 512 B] type:primary id:0x83",
	"number:2 region:[20973568, 41943040, 512 B] type:primary id:0xfd",
    };

    check("/dev/sdc", input, output);
}


BOOST_AUTO_TEST_CASE(parse_broken_json)
{
    vector<string> input = {
	R"({)",
	R"(   "disk": {)",
	R"(      "path": "/dev/sdc",)"
    };

    check_exception("/dev/sdc", input);
}

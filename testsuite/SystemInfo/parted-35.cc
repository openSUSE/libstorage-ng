
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
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --version", RemoteCommand({ "parted (GNU parted) 3.5" }, {}, 0));
    Mockup::set_command(PARTED_BIN " --script --json " + quote(device) + " unit s print",
			RemoteCommand(stdout, stderr, 0));

    Parted parted(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << parted;

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
    Mockup::set_command(PARTED_BIN " --script --json " + quote(device) + " unit s print", input);

    BOOST_CHECK_THROW({ Parted parted(device); }, Exception);
}


BOOST_AUTO_TEST_CASE(parse_msdos)
{
    vector<string> input = {
	"{",
	"   \"disk\": {",
	"      \"path\": \"/dev/sdc\",",
	"      \"size\": \"625142448s\",",
	"      \"model\": \"ATA WDC WD3200BEKT-6\",",
	"      \"transport\": \"scsi\",",
	"      \"logical-sector-size\": 512,",
	"      \"physical-sector-size\": 4096,",
	"      \"label\": \"msdos\",",
	"      \"max-partitions\": 4,",
	"      \"partitions\": [",
	"         {",
	"            \"number\": 1,",
	"            \"start\": \"2048s\",",
	"            \"end\": \"20973567s\",",
	"            \"size\": \"20971520s\",",
	"            \"type\": \"primary\",",
	"            \"type-id\": \"0x83\"",
	"         },{",
	"            \"number\": 2,",
	"            \"start\": \"20973568s\",",
	"            \"end\": \"83888127s\",",
	"            \"size\": \"62914560s\",",
	"            \"type\": \"extended\",",
	"            \"type-id\": \"0x0f\",",
	"            \"flags\": [",
	"                \"lba\"",
	"            ]",
	"         },{",
	"            \"number\": 5,",
	"            \"start\": \"20975616s\",",
	"            \"end\": \"62918655s\",",
	"            \"size\": \"41943040s\",",
	"            \"type\": \"logical\",",
	"            \"type-id\": \"0xfd\",",
	"            \"flags\": [",
	"                \"raid\"",
	"            ]",
	"         }",
	"      ]",
	"   }",
	"}"
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
	"{",
	"   \"disk\": {",
	"      \"path\": \"/dev/sdc\",",
	"      \"size\": \"625142448s\",",
	"      \"model\": \"ATA WDC WD3200BEKT-6\",",
	"      \"transport\": \"scsi\",",
	"      \"logical-sector-size\": 512,",
	"      \"physical-sector-size\": 4096,",
	"      \"label\": \"gpt\",",
	"      \"max-partitions\": 128,",
	"      \"partitions\": [",
	"         {",
	"            \"number\": 1,",
	"            \"start\": \"2048s\",",
	"            \"end\": \"20973567s\",",
	"            \"size\": \"20971520s\",",
	"            \"type\": \"primary\",",
	"            \"type-uuid\": \"0fc63daf-8483-4772-8e79-3d69d8477de4\"",
	"         },{",
	"            \"number\": 2,",
	"            \"start\": \"20973568s\",",
	"            \"end\": \"62916607s\",",
	"            \"size\": \"41943040s\",",
	"            \"type\": \"primary\",",
	"            \"type-uuid\": \"a19d880f-05fc-4d3b-a006-743f0f84911e\",",
	"            \"flags\": [",
	"                \"raid\"",
	"            ]",
	"         }",
	"      ]",
	"   }",
	"}"
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
	"{",
	"   \"disk\": {",
	"      \"path\": \"/dev/sdc\","
    };

    check_exception("/dev/sdc", input);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(MDADMBIN " --detail " + quote(device) + " --export", input);

    MdadmDetail mdadmdetail(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << mdadmdetail;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"MD_LEVEL=raid1",
	"MD_DEVICES=2",
	"MD_METADATA=1.0",
	"MD_UUID=35dd06d4:b4e9e248:9262c3ad:02b61654",
	"MD_NAME=linux:0",
	"MD_DEVICE_sda1_ROLE=0",
	"MD_DEVICE_sda1_DEV=/dev/sda1",
	"MD_DEVICE_sdb1_ROLE=1",
	"MD_DEVICE_sdb1_DEV=/dev/sdb1",
    };

    vector<string> output = {
	"device:/dev/md0 uuid:35dd06d4:b4e9e248:9262c3ad:02b61654 devname: metadata:1.0 level:RAID1"
    };

    check("/dev/md0", input, output);
}

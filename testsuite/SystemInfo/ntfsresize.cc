
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdNtfsresize.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ NTFSRESIZE_BIN, "--force", "--info", device }, input);

    CmdNtfsresize cmd_ntfsresize(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_ntfsresize;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"ntfsresize v2017.3.23 (libntfs-3g)",
	"Device name        : /dev/sdc1",
	"NTFS volume version: 3.1",
	"Cluster size       : 4096 bytes",
	"Current volume size: 4998558208 bytes (4999 MB)",
	"Current device size: 4998561792 bytes (4999 MB)",
	"Checking filesystem consistency ...",
	"  0.00 percent completed\r100.00 percent completed",
	"Accounting clusters ...",
	"Space in use       : 26 MB (0.5%)",
	"Collecting resizing constraints ...",
	"You might resize at 25608192 bytes or 26 MB (freeing 4973 MB).",
	"Please make a test run using both the -n and -s options before real resizing!"
    };

    vector<string> output = {
	"device:/dev/sdc1 min-size:25608192"
    };

    check("/dev/sdc1", input, output);
}


BOOST_AUTO_TEST_CASE(error1)
{
    string device = "/dev/sdc1";

    vector<string> input = {
	"ntfsresize v2017.3.23 (libntfs-3g)",
	"ERROR: Device '/dev/sdc1' is mounted read-write. You must 'umount' it first."
    };

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ NTFSRESIZE_BIN, "--force", "--info", device }, Mockup::Command(input, {}, 1));

    BOOST_CHECK_THROW({ CmdNtfsresize cmd_ntfsresize(device); }, Exception);
}

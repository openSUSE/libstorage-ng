
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdResize2fs.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const string& device, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(RESIZE2FS_BIN " -P " + quote(device), input);

    CmdResize2fs cmd_resize2fs(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_resize2fs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    // Note: The value is above 2^32.

    vector<string> input = {
	"Estimated minimum size of the filesystem: 10000000000"
    };

    vector<string> output = {
	"device:/dev/sdc1 min-blocks:10000000000"
    };

    check("/dev/sdc1", input, output);
}

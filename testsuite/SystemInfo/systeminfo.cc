
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(exception1)
{
    // Check that the correct exception class is thrown (after caching the
    // exception in SystemInfoImpl.h).

    vector<string> input = {
	"WRONG;",
	"/dev/sda:3001GB:scsi:512:4096:gpt:WD My Passport 25E2:;"
    };

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PARTED_BIN " --version", RemoteCommand({ "parted (GNU parted) 3.4" }, {}, 0));
    Mockup::set_command({ PARTED_BIN, "--script", "--machine", "/dev/sda", "unit", "s", "print" }, input);

    SystemInfo::Impl system_info;

    BOOST_CHECK_THROW({ system_info.getParted("/dev/sda"); }, ParseException);

    BOOST_CHECK_THROW({ system_info.getParted("/dev/sda"); }, ParseException);
}

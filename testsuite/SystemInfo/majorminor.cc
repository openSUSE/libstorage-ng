
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/DevAndSys.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const string& device, const string& stdout, const string& result)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::Command command;
    command.stdout = { stdout };

    Mockup::set_command(STATBIN " --dereference --format '%F 0x%t:0x%T' " + quote(device), command);

    MajorMinor majorminor(device);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << majorminor;

    string lhs = parsed.str();
    string rhs = result;

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    string device = "/dev/sda1";
    string stdout = "block special file 0x8:0x1";
    string result = "device:/dev/sda1 majorminor:8:1";

    check(device, stdout, result);
}


BOOST_AUTO_TEST_CASE(error1)
{
    string device = "/dev/mapper/control";

    Mockup::Command command;
    command.stdout = { "character special file 0xa:0xec" };

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(STATBIN " --dereference --format '%F 0x%t:0x%T' " + quote(device), command);

    BOOST_CHECK_THROW(MajorMinor majorminor(device), runtime_error);
}


BOOST_AUTO_TEST_CASE(error2)
{
    string device = "/dev/sda123";

    Mockup::Command command;
    command.stderr = { "stat: cannot stat ‘/dev/sda123’: No such file or directory" };
    command.exit_code = 1;

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(STATBIN " --dereference --format '%F 0x%t:0x%T' " + quote(device), command);

    BOOST_CHECK_THROW(MajorMinor majorminor(device), runtime_error);
}

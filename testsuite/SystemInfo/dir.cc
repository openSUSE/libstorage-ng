
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
check(const string& path, const vector<string>& stdout, const string& result)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::Command command;
    command.stdout = stdout;

    Mockup::set_command({ LS_BIN, "-1", "--sort=none", path }, command);

    Dir dir(path);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << dir;

    string lhs = parsed.str();
    string rhs = result + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    string path = "/sys/block";
    vector<string> stdout = { "sda", "sr" };
    string result = "path:/sys/block entries:<sda sr>";

    check(path, stdout, result);
}


BOOST_AUTO_TEST_CASE(error1)
{
    string path = "/does-not-exist";

    Mockup::Command command;
    command.stderr = { "ls: cannot access /does-not-exist: No such file or directory" };
    command.exit_code = 2;

    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ LS_BIN, "-1", "--sort=none", path }, command);

    BOOST_CHECK_THROW(Dir dir(path), Exception);
}

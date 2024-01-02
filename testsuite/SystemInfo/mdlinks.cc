
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <numeric>
#include <boost/test/unit_test.hpp>

#include "storage/SystemInfo/DevAndSys.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ LS_BIN, "-1l", "--sort=none", "/dev/md" }, input);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    MdLinks md_links(udevadm);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << md_links;

    string lhs = parsed.str();
    string rhs = accumulate(output.begin(), output.end(), ""s,
                            [](auto a, auto b) { return a + b + "\n"; });

    BOOST_CHECK_EQUAL(lhs, rhs);
}

void
check_error(const vector<string>& input, const vector<string>& error_input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::Command command(input, error_input, 1);
    Mockup::set_command({ LS_BIN, "-1l", "--sort=none", "/dev/md" }, command);
    Mockup::set_command({ UDEVADM_BIN_SETTLE }, {});

    Udevadm udevadm;

    MdLinks md_links(udevadm);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << md_links;

    string lhs = parsed.str();
    string rhs = accumulate(output.begin(), output.end(), ""s,
                            [](auto a, auto b) { return a + b + "\n"; });

    BOOST_CHECK_EQUAL(lhs, rhs);
}

BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"total 0",
	"lrwxrwxrwx 1 root root 8 Jan 13 16:02 test -> ../md127"
    };

    vector<string> output = {
	"data[md127] -> test",
    };

    check(input, output);
}

BOOST_AUTO_TEST_CASE(parse_missing)
{
    vector<string> input = {};

    vector<string> error_input = {
        "/bin/ls: cannot access '/dev/md': No such file or directory"
    };

    vector<string> output = {};

    check_error(input, error_input, output);
}


// TODO tests with strange characters in paths

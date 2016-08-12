
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdDmsetup.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DMSETUPBIN " table", input);

    CmdDmsetupTable cmddmsetuptable;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmddmsetuptable;

    string lhs = parsed.str();
    string rhs = output.empty() ? "" : boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"system-luks: 0 2097152 linear 8:2 593496064",
	"cr_luks: 0 2093056 crypt aes-xts-plain64 0000000000000000000000000000000000000000000000000000000000000000 0 254:12 4096",
	"system-root: 0 33554432 linear 8:2 125831168",
	"system-root: 33554432 33554432 linear 8:2 897632256"
    };

    vector<string> output = {
	"data[cr_luks] -> target:crypt majorminors:<254:12>",
	"data[system-luks] -> target:linear majorminors:<8:2>",
	"data[system-root] -> target:linear majorminors:<8:2>",
	"data[system-root] -> target:linear majorminors:<8:2>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"test-fast: 0 409600 striped 2 128 8:17 2048 8:18 2048"
    };

    vector<string> output = {
	"data[test-fast] -> target:striped stripes:2 stripe-size:65536 majorminors:<8:17 8:18>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse_no_devices)
{
    vector<string> input = {
	"No devices found"
    };

    vector<string> output = {
    };

    check(input, output);
}

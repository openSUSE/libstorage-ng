
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
    string rhs = boost::join(output, "\n") + "\n";

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

    // TODO output format

    vector<string> output = {
	"data[cr_luks] -> majorminors:<254:12>",
	"data[system-luks] -> majorminors:<8:2>",
	"data[system-root] -> majorminors:<8:2 8:2>"
    };

    check(input, output);
}

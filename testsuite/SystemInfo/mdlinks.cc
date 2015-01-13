
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

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
    Mockup::set_command(LSBIN " -1l --sort=none " + quote("/dev/md"), input);

    MdLinks mdlinks;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << mdlinks;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

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


// TODO tests with strange characters in paths

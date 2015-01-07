
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdLvm.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_command(VGSBIN " --noheadings --unbuffered --options vg_name", input);

    CmdVgs cmdvgs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmdvgs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  system",
	"  data"
    };

    vector<string> output = {
	"vgs:<system data>"
    };

    check(input, output);
}

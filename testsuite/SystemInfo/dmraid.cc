
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdDmraid.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DMRAIDBIN " -s -c -c -c", input);

    CmdDmraid cmddmraid;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmddmraid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"isw_bfhghbahji_foo:16767744:128:mirror:ok:0:2:0",
	"/dev/sda:isw:isw_bfhghbahji_foo:mirror:ok:16767744:0",
	"/dev/sdb:isw:isw_bfhghbahji_foo:mirror:ok:16767744:0"
    };

    vector<string> output = {
	"data[isw_bfhghbahji_foo] -> raid-type:mirror controller:isw devices:</dev/sda /dev/sdb>"
    };

    check(input, output);
}

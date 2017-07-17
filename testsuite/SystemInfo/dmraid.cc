
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
	"isw_ddgdcbibhd_test1:16777216:128:mirror:ok:0:2:0",
	"/dev/sdb:isw:isw_ddgdcbibhd_test1:mirror:ok:16777216:0",
	"/dev/sdc:isw:isw_ddgdcbibhd_test1:mirror:ok:16777216:0",
	"isw_ddgdcbibhd_test2:33534976:256:stripe:ok:0:2:0",
	"/dev/sdb:isw:isw_ddgdcbibhd_test2:stripe:ok:16767488:16781576",
	"/dev/sdc:isw:isw_ddgdcbibhd_test2:stripe:ok:16767488:16781576"
    };

    vector<string> output = {
	"data[isw_ddgdcbibhd_test1] -> raid-type:mirror controller:isw devices:</dev/sdb /dev/sdc>",
	"data[isw_ddgdcbibhd_test2] -> raid-type:stripe controller:isw devices:</dev/sdb /dev/sdc>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"isw_ebihadbfhi_test1:100634880:128:raid5_la:ok:0:4:0",
	"/dev/sdb:isw:isw_ebihadbfhi_test1:raid5_la:ok:33544960:0",
	"/dev/sdc:isw:isw_ebihadbfhi_test1:raid5_la:ok:33544960:0",
	"/dev/sdd:isw:isw_ebihadbfhi_test1:raid5_la:ok:33544960:0",
	"/dev/sde:isw:isw_ebihadbfhi_test1:raid5_la:ok:33544960:0"
    };

    vector<string> output = {
	"data[isw_ebihadbfhi_test1] -> raid-type:raid5_la controller:isw devices:</dev/sdb /dev/sdc /dev/sdd /dev/sde>"
    };

    check(input, output);
}

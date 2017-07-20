
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

    CmdDmraid cmd_dmraid;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_dmraid;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check_exception(const vector<string>& input)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DMRAIDBIN " -s -c -c -c", input);

    BOOST_CHECK_THROW({ CmdDmraid cmd_dmraid; }, Exception);
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
	"isw_bjaecefeha_test2:67089920:256:stripe:ok:0:2:0",
	"/dev/sdd:isw:isw_bjaecefeha_test2:stripe:ok:33544960:0",
	"/dev/sde:isw:isw_bjaecefeha_test2:stripe:ok:33544960:0",
	"isw_ddjgjiiabg_test1:33544960:128:mirror:ok:0:2:0",
	"/dev/sdb:isw:isw_ddjgjiiabg_test1:mirror:ok:33544960:0",
	"/dev/sdc:isw:isw_ddjgjiiabg_test1:mirror:ok:33544960:0"
    };

    vector<string> output = {
	"data[isw_bjaecefeha_test2] -> raid-type:stripe controller:isw devices:</dev/sdd /dev/sde>",
	"data[isw_ddjgjiiabg_test1] -> raid-type:mirror controller:isw devices:</dev/sdb /dev/sdc>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
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


BOOST_AUTO_TEST_CASE(parse4)
{
    vector<string> input = {
	"isw_eaifiecdfd_test:67089920:128:raid01:ok:2:4:0",
	"isw_eaifiecdfd_test-0:33544960:128:mirror:ok:0:2:0",
	"/dev/sdb:isw:isw_eaifiecdfd_test-0:mirror:ok:33544960:0",
	"/dev/sdc:isw:isw_eaifiecdfd_test-0:mirror:ok:33544960:0",
	"isw_eaifiecdfd_test-1:33544960:128:mirror:ok:0:2:0",
	"/dev/sdd:isw:isw_eaifiecdfd_test-1:mirror:ok:33544960:0",
	"/dev/sde:isw:isw_eaifiecdfd_test-1:mirror:ok:33544960:0"
    };

    vector<string> output = {
	"data[isw_eaifiecdfd_test] -> raid-type:raid01 controller:isw devices:</dev/sdb /dev/sdc /dev/sdd /dev/sde>"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse5)
{
    // error: lines missing

    vector<string> input = {
	"isw_ddjgjiiabg_test1:33544960:128:mirror:ok:0:2:0",
	"/dev/sdb:isw:isw_ddjgjiiabg_test1:mirror:ok:33544960:0"
    };

    check_exception(input);
}


BOOST_AUTO_TEST_CASE(parse6)
{
    // error: fields missing

    vector<string> input = {
	"isw_ddjgjiiabg_test1:33544960",
	"/dev/sdb:isw:isw_ddjgjiiabg_test1:mirror:ok:33544960:0",
	"/dev/sdc:isw:isw_ddjgjiiabg_test1:mirror:ok:33544960:0"
    };

    check_exception(input);
}

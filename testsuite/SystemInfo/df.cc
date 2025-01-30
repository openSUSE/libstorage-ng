
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/CmdDf.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(DF_BIN " --block-size=1 --output=size,used,avail,fstype /test", input);

    CmdDf cmd_df("/test");

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_df;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  1B-blocks    Used       Avail Type",
	"10738466816 7864320 10183770112 btrfs"
    };

    vector<string> output = {
	"path:/test size:10738466816 used:7864320 available:10183770112 fs-type:btrfs"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"   1B-blocks        Used        Avail Type",
	"983348543488 61565304832 871756464128 nfs4"
    };

    vector<string> output = {
	"path:/test size:983348543488 used:61565304832 available:871756464128 fs-type:nfs4"
    };

    check(input, output);
}

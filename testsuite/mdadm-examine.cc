
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>

#include "storage/SystemInfo/ProcMdstat.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


void
check(const list<string>& devices, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_command(MDADMBIN " --examine " + quote(devices) + " --brief", input);

    MdadmExamine mdadmdetail(devices);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << mdadmdetail;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"ARRAY /dev/md/0  metadata=1.0 UUID=35dd06d4:b4e9e248:9262c3ad:02b61654 name=linux:0"
    };

    vector<string> output = {
	"devices:</dev/sda1 /dev/sdb1> metadata:1.0 uuid:35dd06d4:b4e9e248:9262c3ad:02b61654"
    };

    check({ "/dev/sda1", "/dev/sdb1" }, input, output);
}

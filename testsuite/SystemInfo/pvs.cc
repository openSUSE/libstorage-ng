
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
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(PVSBIN " --noheadings --unbuffered --units b --nosuffix --options pv_name,"
			"pv_uuid,vg_name,vg_uuid", input);

    CmdPvs cmd_pvs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_pvs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  /dev/sda2  qquP1O-WWoh-Ofas-Rbx0-y72T-0sNe-Wnyc33 system OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn"
    };

    vector<string> output = {
	"pvs:<pv-name:/dev/sda2 pv-uuid:qquP1O-WWoh-Ofas-Rbx0-y72T-0sNe-Wnyc33 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn>"
    };

    check(input, output);
}

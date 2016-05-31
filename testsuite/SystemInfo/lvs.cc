
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
    Mockup::set_command(LVSBIN " --noheadings --unbuffered --options lv_name,lv_uuid,vg_name,vg_uuid", input);

    CmdLvs cmd_lvs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_lvs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n");

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  root 89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN system OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn",
	"  swap KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 system OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn"
    };

    // TODO bad output format

    vector<string> output = {
	"lvs:<lv-name:root lv-uuid:89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-name:swap lv-uuid:KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn>"
    };

    check(input, output);
}

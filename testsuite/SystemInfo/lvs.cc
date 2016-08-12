
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
    Mockup::set_command(LVSBIN " --noheadings --unbuffered --units b --nosuffix --options lv_name,"
			"lv_uuid,vg_name,vg_uuid,lv_attr,lv_size", input);

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
	"  root 89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN system OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn -wi-ao---- 34359738368",
	"  swap KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 system OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn -wi-ao---- 2147483648"
    };

    // TODO bad output format

    vector<string> output = {
	"lvs:<lv-name:root lv-uuid:89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn active:true size:34359738368 lv-name:swap lv-uuid:KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn active:true size:2147483648>"
    };

    check(input, output);
}

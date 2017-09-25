
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
    Mockup::set_command(LVSBIN " --reportformat json --units b --nosuffix --options lv_name,"
			"lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,lv_size,pool_lv,pool_lv_uuid",
			input);

    CmdLvs cmd_lvs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_lvs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


BOOST_AUTO_TEST_CASE(parse1)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"root\", \"lv_uuid\":\"89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN\", \"vg_name\":\"system\", \"vg_uuid\":\"OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-ao----\", \"lv_size\":\"34359738368\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"swap\", \"lv_uuid\":\"KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0\", \"vg_name\":\"system\", \"vg_uuid\":\"OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-ao----\", \"lv_size\":\"2147483648\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    // TODO bad output format

    vector<string> output = {
	"lv:{ lv-name:root lv-uuid:89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal active:true size:34359738368 }",
	"lv:{ lv-name:swap lv-uuid:KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal active:true size:2147483648 }"
    };

    check(input, output);
}

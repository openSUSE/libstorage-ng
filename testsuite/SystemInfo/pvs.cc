
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
    Mockup::set_command({ PVS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--all", "--options", "pv_name,pv_uuid,vg_name,vg_uuid,pv_attr,pe_start" },
	    input);

    CmdPvs cmd_pvs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_pvs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& pv_name, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ PVS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--all", "--options", "pv_name,pv_uuid,vg_name,vg_uuid,pv_attr,pe_start",
	    pv_name }, input);

    CmdPvs cmd_pvs(pv_name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_pvs;

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
	"              \"pv\": [",
	"                  {\"pv_name\":\"/dev/sda2\", \"pv_uuid\":\"qquP1O-WWoh-Ofas-Rbx0-y72T-0sNe-Wnyc33\", \"vg_name\":\"system\", \"vg_uuid\":\"OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn\", \"pv_attr\":\"a--\", \"pe_start\":\"1048576\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"pv:{ pv-name:/dev/sda2 pv-uuid:qquP1O-WWoh-Ofas-Rbx0-y72T-0sNe-Wnyc33 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn pe-start:1048576 }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    // The device reported pv name can be different from the provided pv name.

    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"pv\": [",
	"                  {\"pv_name\":\"/dev/md127\", \"pv_uuid\":\"nA8nKb-VSgN-fRvo-pGqm-pRBl-MX3M-Hf2sjT\", \"vg_name\":\"\", \"vg_uuid\":\"\", \"pv_attr\":\"---\", \"pe_start\":\"1048576\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"pv:{ pv-name:/dev/md127 pv-uuid:nA8nKb-VSgN-fRvo-pGqm-pRBl-MX3M-Hf2sjT vg-name: vg-uuid: pe-start:1048576 }"
    };

    check("/dev/md/a", input, output);
}


BOOST_AUTO_TEST_CASE(parse_missing)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"pv\": [",
	"                  {\"pv_name\":\"/dev/sda1\", \"pv_uuid\":\"smMwqo-zDbE-fa6S-ouy2-MmJN-pxlg-VOjrOX\", \"vg_name\":\"test\", \"vg_uuid\":\"dl6cxn-1P1U-SWjT-Ckhq-viMD-GF9z-SU9XvE\", \"pv_attr\":\"a--\", \"pe_start\":\"1048576\"},",
	"                  {\"pv_name\":\"[unknown]\", \"pv_uuid\":\"vfbt1o-rEi2-ph1Q-ksMt-qhTw-4lwl-iruKqS\", \"vg_name\":\"test\", \"vg_uuid\":\"dl6cxn-1P1U-SWjT-Ckhq-viMD-GF9z-SU9XvE\", \"pv_attr\":\"a-m\", \"pe_start\":\"1048576\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"pv:{ pv-name:/dev/sda1 pv-uuid:smMwqo-zDbE-fa6S-ouy2-MmJN-pxlg-VOjrOX vg-name:test vg-uuid:dl6cxn-1P1U-SWjT-Ckhq-viMD-GF9z-SU9XvE pe-start:1048576 }",
	"pv:{ pv-name:[unknown] pv-uuid:vfbt1o-rEi2-ph1Q-ksMt-qhTw-4lwl-iruKqS vg-name:test vg-uuid:dl6cxn-1P1U-SWjT-Ckhq-viMD-GF9z-SU9XvE pe-start:1048576 missing }"
    };

    check(input, output);
}

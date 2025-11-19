
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
    Mockup::set_command({ VGS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--options", "vg_name,vg_uuid,vg_attr,vg_extent_size,"
	    "vg_extent_count,vg_free_count" }, input);

    CmdVgs cmd_vgs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_vgs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& vg_name, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ VGS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--options", "vg_name,vg_uuid,vg_attr,vg_extent_size,"
	    "vg_extent_count,vg_free_count", "--", vg_name }, input);

    CmdVgs cmd_vgs(vg_name);

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_vgs;

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
	"              \"vg\": [",
	"                  {\"vg_name\":\"system\", \"vg_uuid\":\"OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn\", \"vg_attr\":\"wz--n-\", \"vg_extent_size\":\"4194304\", \"vg_extent_count\":\"230400\", \"vg_free_count\":\"71666\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"vg:{ vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn extent-size:4194304 extent-count:230400 free-extent-count:71666 }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"vg\": [",
	"                  {\"vg_name\":\"b\", \"vg_uuid\":\"GbcwxM-Px5E-Xs7u-dLhx-r7RU-4LG4-uGR6Ew\", \"vg_attr\":\"wz--n-\", \"vg_extent_size\":\"4194304\", \"vg_extent_count\":\"25640\", \"vg_free_count\":\"25640\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"vg:{ vg-name:b vg-uuid:GbcwxM-Px5E-Xs7u-dLhx-r7RU-4LG4-uGR6Ew extent-size:4194304 extent-count:25640 free-extent-count:25640 }"
    };

    check("b", input, output);
}

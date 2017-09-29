
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
    Mockup::set_command(LVSBIN " --reportformat json --units b --nosuffix --all --options lv_name,"
			"lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,lv_size,stripes,stripe_size,"
			"chunk_size,pool_lv,pool_lv_uuid,data_lv,data_lv_uuid,metadata_lv,"
			"metadata_lv_uuid", input);

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

    vector<string> output = {
	"lv:{ lv-name:root lv-uuid:89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal active:true size:34359738368 }",
	"lv:{ lv-name:swap lv-uuid:KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal active:true size:2147483648 }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse2)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"linear\", \"lv_uuid\":\"dlSOFR-1IMP-RiWe-48Pl-3Qnb-WX5b-TVFjYW\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"104857600\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"striped\", \"lv_uuid\":\"RECoSq-x9Hg-895X-PB4a-mowu-p4hJ-cSzmJi\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"109051904\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},"
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:linear lv-uuid:dlSOFR-1IMP-RiWe-48Pl-3Qnb-WX5b-TVFjYW vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:normal active:true size:104857600 }",
	"lv:{ lv-name:striped lv-uuid:RECoSq-x9Hg-895X-PB4a-mowu-p4hJ-cSzmJi vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:normal active:true size:109051904 }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse3)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"[lvol0_pmspare]\", \"lv_uuid\":\"JJBOpd-hNtd-WXVM-Eoxw-qxWW-0WJZ-FZMBDS\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"private,pool,spare\", \"lv_attr\":\"ewi-------\", \"lv_size\":\"4194304\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin-pool\", \"lv_uuid\":\"YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"private\", \"lv_attr\":\"twi-aotz--\", \"lv_size\":\"1073741824\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[thin-pool_tdata]\", \"lv_uuid\":\"WNYD6Z-tPJM-QmjG-3bqA-elOR-oNti-azR4wY\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"private,thin,pool,data\", \"lv_attr\":\"Twi-ao----\", \"lv_size\":\"1073741824\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[thin-pool_tmeta]\", \"lv_uuid\":\"s9FXve-chCq-TPNR-BnvO-iX0i-JJXO-gTyIKk\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"private,thin,pool,metadata\", \"lv_attr\":\"ewi-ao----\", \"lv_size\":\"4194304\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin1\", \"lv_uuid\":\"nP2GgC-bMc6-D6cn-Yj8k-fkE8-Z9Qw-O4KPk6\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"public\", \"lv_attr\":\"Vwi-a-tz--\", \"lv_size\":\"2147483648\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV\"},",
	"                  {\"lv_name\":\"thin2\", \"lv_uuid\":\"SFGved-huXH-LpQF-PQNn-B3UP-WM74-h3lD1p\", \"vg_name\":\"test\", \"vg_uuid\":\"1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw\", \"lv_role\":\"public\", \"lv_attr\":\"Vwi-a-tz--\", \"lv_size\":\"3221225472\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV\"}"
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:[lvol0_pmspare] lv-uuid:JJBOpd-hNtd-WXVM-Eoxw-qxWW-0WJZ-FZMBDS vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown active:false size:4194304 }",
	"lv:{ lv-name:[thin-pool_tdata] lv-uuid:WNYD6Z-tPJM-QmjG-3bqA-elOR-oNti-azR4wY vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown active:true size:1073741824 }",
	"lv:{ lv-name:[thin-pool_tmeta] lv-uuid:s9FXve-chCq-TPNR-BnvO-iX0i-JJXO-gTyIKk vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown active:true size:4194304 }",
	"lv:{ lv-name:thin-pool lv-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin-pool active:true size:1073741824 }",
	"lv:{ lv-name:thin1 lv-uuid:nP2GgC-bMc6-D6cn-Yj8k-fkE8-Z9Qw-O4KPk6 vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin active:true size:2147483648 pool-name:thin-pool pool-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV }",
	"lv:{ lv-name:thin2 lv-uuid:SFGved-huXH-LpQF-PQNn-B3UP-WM74-h3lD1p vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin active:true size:3221225472 pool-name:thin-pool pool-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV }"
    };

    check(input, output);
}

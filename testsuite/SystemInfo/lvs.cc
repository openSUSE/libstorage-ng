
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
    Mockup::set_command({ LVS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--all", "--options lv_name,lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,"
	    "lv_size,origin_size,segtype,stripes,stripe_size,chunk_size,pool_lv,pool_lv_uuid,origin,origin_uuid,"
	    "data_lv,data_lv_uuid,metadata_lv,metadata_lv_uuid" }, input);

    CmdLvs cmd_lvs;

    ostringstream parsed;
    parsed.setf(std::ios::boolalpha);
    parsed << cmd_lvs;

    string lhs = parsed.str();
    string rhs = boost::join(output, "\n") + "\n";

    BOOST_CHECK_EQUAL(lhs, rhs);
}


void
check(const string& vg_name, const string& lv_name, const vector<string>& input, const vector<string>& output)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command({ LVS_BIN, "--reportformat", "json", "--config", "log { command_names = 0 prefix = \"\" }",
	    "--units", "b", "--nosuffix", "--all", "--options lv_name,lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,"
	    "lv_size,origin_size,segtype,stripes,stripe_size,chunk_size,pool_lv,pool_lv_uuid,origin,origin_uuid,"
	    "data_lv,data_lv_uuid,metadata_lv,metadata_lv_uuid", "--", vg_name + "/" + lv_name }, input);

    CmdLvs cmd_lvs(vg_name, lv_name);

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
	"lv:{ lv-name:root lv-uuid:89Crg8-K5dO-0Vvj-Vwur-vCLK-4efh-WCtRfN vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal role:public active:true size:34359738368 segments:<> }",
	"lv:{ lv-name:swap lv-uuid:KKC5tf-bWLp-sF2t-oVKQ-tE0w-xeQp-Up8bV0 vg-name:system vg-uuid:OMPzXF-m3am-1zIl-AVdQ-i5Wx-tmyN-cevmRn lv-type:normal role:public active:true size:2147483648 segments:<> }"
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
	"lv:{ lv-name:linear lv-uuid:dlSOFR-1IMP-RiWe-48Pl-3Qnb-WX5b-TVFjYW vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:normal role:public active:true size:104857600 segments:<> }",
	"lv:{ lv-name:striped lv-uuid:RECoSq-x9Hg-895X-PB4a-mowu-p4hJ-cSzmJi vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:normal role:public active:true size:109051904 segments:<> }"
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
	"lv:{ lv-name:[lvol0_pmspare] lv-uuid:JJBOpd-hNtd-WXVM-Eoxw-qxWW-0WJZ-FZMBDS vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown role:private active:false size:4194304 segments:<> }",
	"lv:{ lv-name:[thin-pool_tdata] lv-uuid:WNYD6Z-tPJM-QmjG-3bqA-elOR-oNti-azR4wY vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown role:private active:true size:1073741824 segments:<> }",
	"lv:{ lv-name:[thin-pool_tmeta] lv-uuid:s9FXve-chCq-TPNR-BnvO-iX0i-JJXO-gTyIKk vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:unknown role:private active:true size:4194304 segments:<> }",
	"lv:{ lv-name:thin-pool lv-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin-pool role:private active:true size:1073741824 segments:<> }",
	"lv:{ lv-name:thin1 lv-uuid:nP2GgC-bMc6-D6cn-Yj8k-fkE8-Z9Qw-O4KPk6 vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin role:public active:true size:2147483648 pool-name:thin-pool pool-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV segments:<> }",
	"lv:{ lv-name:thin2 lv-uuid:SFGved-huXH-LpQF-PQNn-B3UP-WM74-h3lD1p vg-name:test vg-uuid:1b9W8v-TNjH-1ca2-bR2T-43mZ-n5h9-NDpxBw lv-type:thin role:public active:true size:3221225472 pool-name:thin-pool pool-uuid:YKs2Du-ajnO-VGzP-graH-Ps1t-MO1l-ZvKkQV segments:<> }"
    };

    check(input, output);
}



BOOST_AUTO_TEST_CASE(parse4)
{
    /*
     * The LV here has several segments with different stripes and
     * stripe-size. This is possible to create by using lvresize and providing
     * stripes and stripesize. Without options lvresize uses the values from
     * the last segment so different values should be a rather exotic case.
     */

    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"normal\", \"lv_uuid\":\"3Kzffs-MSVL-qrEM-1Oca-t286-VtBJ-VIa2Xw\", \"vg_name\":\"test\", \"vg_uuid\":\"VVCY2u-fXvO-k2oG-hGOO-ezEb-Hzfr-lujlRx\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"25769803776\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"normal\", \"lv_uuid\":\"3Kzffs-MSVL-qrEM-1Oca-t286-VtBJ-VIa2Xw\", \"vg_name\":\"test\", \"vg_uuid\":\"VVCY2u-fXvO-k2oG-hGOO-ezEb-Hzfr-lujlRx\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"25769803776\", \"stripes\":\"2\", \"stripe_size\":\"65536\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"normal\", \"lv_uuid\":\"3Kzffs-MSVL-qrEM-1Oca-t286-VtBJ-VIa2Xw\", \"vg_name\":\"test\", \"vg_uuid\":\"VVCY2u-fXvO-k2oG-hGOO-ezEb-Hzfr-lujlRx\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"25769803776\", \"stripes\":\"2\", \"stripe_size\":\"262144\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:normal lv-uuid:3Kzffs-MSVL-qrEM-1Oca-t286-VtBJ-VIa2Xw vg-name:test vg-uuid:VVCY2u-fXvO-k2oG-hGOO-ezEb-Hzfr-lujlRx lv-type:normal role:public active:true size:25769803776 segments:<stripes:1 stripes:2 stripe-size:65536 stripes:2 stripe-size:262144> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse5)
{
    // "cache1" is using a cache volume, "cache2" is using the cache
    // pool "cache2-pool", "cache3" is detached from the cache pool
    // "cache3-pool".

    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"cache1\", \"lv_uuid\":\"R3wAkS-5BJp-VwEe-DS9r-gBJG-jlgB-MvwUGG\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"public\", \"lv_attr\":\"Cwi-a-C---\", \"lv_size\":\"10737418240\", \"segtype\":\"cache\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"[cache1-volume]\", \"pool_lv_uuid\":\"su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache1-volume]\", \"lv_uuid\":\"su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private\", \"lv_attr\":\"Cwi-aoC---\", \"lv_size\":\"1073741824\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache1_corig]\", \"lv_uuid\":\"ZzFhN5-b2N6-dL4F-H42U-dAP9-laF2-EU1F3p\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,origin,cacheorigin\", \"lv_attr\":\"owi-aoC---\", \"lv_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache2\", \"lv_uuid\":\"N123dc-RYSs-YEAG-KcLk-kgki-Dvlx-Iwgj3L\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"public\", \"lv_attr\":\"Cwi-a-C---\", \"lv_size\":\"10737418240\", \"segtype\":\"cache\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"[cache2-pool]\", \"pool_lv_uuid\":\"DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2-pool]\", \"lv_uuid\":\"DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private\", \"lv_attr\":\"Cwi---C---\", \"lv_size\":\"1073741824\", \"segtype\":\"cache-pool\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"[cache2-pool_cdata]\", \"data_lv_uuid\":\"Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q\", \"metadata_lv\":\"[cache2-pool_cmeta]\", \"metadata_lv_uuid\":\"3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi\"},",
	"                  {\"lv_name\":\"[cache2-pool_cdata]\", \"lv_uuid\":\"Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,pool,data\", \"lv_attr\":\"Cwi-ao----\", \"lv_size\":\"1073741824\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2-pool_cmeta]\", \"lv_uuid\":\"3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,pool,metadata\", \"lv_attr\":\"ewi-ao----\", \"lv_size\":\"8388608\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2_corig]\", \"lv_uuid\":\"qhafQi-Nznt-WJCP-bMpl-Hx1x-w8tb-kt0EEa\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,origin,cacheorigin\", \"lv_attr\":\"owi-aoC---\", \"lv_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache3\", \"lv_uuid\":\"e0Lwjb-3rMk-RwYB-OWYf-YInP-H1UJ-1HpYtJ\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache3-pool\", \"lv_uuid\":\"vrpci4-gT93-VZZg-Z1DU-YvOr-9E7e-TzXmgO\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private\", \"lv_attr\":\"Cwi---C---\", \"lv_size\":\"1073741824\", \"segtype\":\"cache-pool\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"[cache3-pool_cdata]\", \"data_lv_uuid\":\"BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH\", \"metadata_lv\":\"[cache3-pool_cmeta]\", \"metadata_lv_uuid\":\"fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS\"},",
	"                  {\"lv_name\":\"[cache3-pool_cdata]\", \"lv_uuid\":\"BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,pool,data\", \"lv_attr\":\"Cwi-------\", \"lv_size\":\"1073741824\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache3-pool_cmeta]\", \"lv_uuid\":\"fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,cache,pool,metadata\", \"lv_attr\":\"ewi-------\", \"lv_size\":\"8388608\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[lvol0_pmspare]\", \"lv_uuid\":\"vLO1sd-Slsj-VY6P-sTzJ-ZEya-0hFZ-uAJTuq\", \"vg_name\":\"test\", \"vg_uuid\":\"55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC\", \"lv_role\":\"private,pool,spare\", \"lv_attr\":\"ewi-------\", \"lv_size\":\"8388608\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:[cache1-volume] lv-uuid:su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache1_corig] lv-uuid:ZzFhN5-b2N6-dL4F-H42U-dAP9-laF2-EU1F3p vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool] lv-uuid:DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache-pool role:private active:false size:1073741824 data-name:[cache2-pool_cdata] data-uuid:Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q metadata-name:[cache2-pool_cmeta] metadata-uuid:3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:[cache2-pool_cdata] lv-uuid:Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool_cmeta] lv-uuid:3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2_corig] lv-uuid:qhafQi-Nznt-WJCP-bMpl-Hx1x-w8tb-kt0EEa vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[cache3-pool_cdata] lv-uuid:BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:private active:false size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache3-pool_cmeta] lv-uuid:fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[lvol0_pmspare] lv-uuid:vLO1sd-Slsj-VY6P-sTzJ-ZEya-0hFZ-uAJTuq vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:cache1 lv-uuid:R3wAkS-5BJp-VwEe-DS9r-gBJG-jlgB-MvwUGG vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache role:public active:true size:10737418240 pool-name:[cache1-volume] pool-uuid:su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache2 lv-uuid:N123dc-RYSs-YEAG-KcLk-kgki-Dvlx-Iwgj3L vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache role:public active:true size:10737418240 pool-name:[cache2-pool] pool-uuid:DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache3 lv-uuid:e0Lwjb-3rMk-RwYB-OWYf-YInP-H1UJ-1HpYtJ vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:public active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:cache3-pool lv-uuid:vrpci4-gT93-VZZg-Z1DU-YvOr-9E7e-TzXmgO vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache-pool role:private active:false size:1073741824 data-name:[cache3-pool_cdata] data-uuid:BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH metadata-name:[cache3-pool_cmeta] metadata-uuid:fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS segments:<stripes:1 chunk-size:65536> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse6)
{
    // "linear1" has two thick snapshots, "thin2" has two thin
    // snapshots, "linear3" has one thick and one thin snapshot

    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"linear1\", \"lv_uuid\":\"Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,origin,thickorigin,multithickorigin\", \"lv_attr\":\"owi-a-s---\", \"lv_size\":\"1073741824\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear1-snap1\", \"lv_uuid\":\"4Tj4DY-ivT9-mc5n-lQ6v-DgV6-0N9K-CjuZgH\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"134217728\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"linear1\", \"origin_uuid\":\"Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear1-snap2\", \"lv_uuid\":\"QpDp3Q-4vzu-nsUf-xvEw-TXF9-e49X-ps7Wop\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"134217728\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"linear1\", \"origin_uuid\":\"Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear3\", \"lv_uuid\":\"ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,origin,extthinorigin,thickorigin\", \"lv_attr\":\"ori-a-s---\", \"lv_size\":\"1073741824\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear3-snap1\", \"lv_uuid\":\"aIsi8o-0Mtx-l9xd-1jnr-LxOA-QLAl-1QNSdf\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"134217728\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"linear3\", \"origin_uuid\":\"ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear3-snap2\", \"lv_uuid\":\"CMRFlu-B5Qf-fSiN-0EmQ-zlgS-M4G1-L8ED0u\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thinsnapshot\", \"lv_attr\":\"Vwi-a-tz--\", \"lv_size\":\"1073741824\", \"segtype\":\"thin\", \"stripes\":\"0\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5\", \"origin\":\"linear3\", \"origin_uuid\":\"ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"lvol0\", \"lv_uuid\":\"sH9o8O-vxn2-AWn3-x11h-UfJE-TzHv-Vp0ETU\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-------\", \"lv_size\":\"8388608\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[lvol1_pmspare]\", \"lv_uuid\":\"uI5cNP-D6Uk-QytK-6eZA-9bte-n7JP-ZKeSKc\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"private,pool,spare\", \"lv_attr\":\"ewi-------\", \"lv_size\":\"4194304\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin-pool\", \"lv_uuid\":\"1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"private\", \"lv_attr\":\"twi-aotz--\", \"lv_size\":\"4294967296\", \"segtype\":\"thin-pool\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"[thin-pool_tdata]\", \"data_lv_uuid\":\"GgEqYb-YsrV-HviJ-0SNI-eMDH-q37w-tQRpLh\", \"metadata_lv\":\"[thin-pool_tmeta]\", \"metadata_lv_uuid\":\"2GOZZy-rjkB-A0z1-dMrG-oWpv-eJWH-UlhKPw\"},",
	"                  {\"lv_name\":\"[thin-pool_tdata]\", \"lv_uuid\":\"GgEqYb-YsrV-HviJ-0SNI-eMDH-q37w-tQRpLh\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"private,thin,pool,data\", \"lv_attr\":\"Twi-ao----\", \"lv_size\":\"4294967296\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[thin-pool_tmeta]\", \"lv_uuid\":\"2GOZZy-rjkB-A0z1-dMrG-oWpv-eJWH-UlhKPw\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"private,thin,pool,metadata\", \"lv_attr\":\"ewi-ao----\", \"lv_size\":\"4194304\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin2\", \"lv_uuid\":\"fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,origin,thinorigin,multithinorigin\", \"lv_attr\":\"Vwi-a-tz--\", \"lv_size\":\"2147483648\", \"segtype\":\"thin\", \"stripes\":\"0\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin2-snap1\", \"lv_uuid\":\"3oli42-K3HH-dkl8-RNOh-MEpq-2ZKI-3nO1en\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thinsnapshot\", \"lv_attr\":\"Vwi-a-tz-k\", \"lv_size\":\"2147483648\", \"segtype\":\"thin\", \"stripes\":\"0\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5\", \"origin\":\"thin2\", \"origin_uuid\":\"fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"thin2-snap2\", \"lv_uuid\":\"e24hyo-e0py-QNdk-xqdR-n2ZX-NBZb-0MkL5B\", \"vg_name\":\"test\", \"vg_uuid\":\"e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7\", \"lv_role\":\"public,snapshot,thinsnapshot\", \"lv_attr\":\"Vwi---tz-k\", \"lv_size\":\"2147483648\", \"segtype\":\"thin\", \"stripes\":\"0\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"thin-pool\", \"pool_lv_uuid\":\"1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5\", \"origin\":\"thin2\", \"origin_uuid\":\"fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"}"
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:[lvol1_pmspare] lv-uuid:uI5cNP-D6Uk-QytK-6eZA-9bte-n7JP-ZKeSKc vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:unknown role:private active:false size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[thin-pool_tdata] lv-uuid:GgEqYb-YsrV-HviJ-0SNI-eMDH-q37w-tQRpLh vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:unknown role:private active:true size:4294967296 segments:<stripes:1> }",
	"lv:{ lv-name:[thin-pool_tmeta] lv-uuid:2GOZZy-rjkB-A0z1-dMrG-oWpv-eJWH-UlhKPw vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:linear1 lv-uuid:Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:normal role:public active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:linear1-snap1 lv-uuid:4Tj4DY-ivT9-mc5n-lQ6v-DgV6-0N9K-CjuZgH vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:snapshot role:public active:true size:134217728 origin-name:linear1 origin-uuid:Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:linear1-snap2 lv-uuid:QpDp3Q-4vzu-nsUf-xvEw-TXF9-e49X-ps7Wop vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:snapshot role:public active:true size:134217728 origin-name:linear1 origin-uuid:Qf2yM9-IOUP-hCob-kHOW-NcI3-KVr4-EULvzO segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:linear3 lv-uuid:ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:normal role:public active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:linear3-snap1 lv-uuid:aIsi8o-0Mtx-l9xd-1jnr-LxOA-QLAl-1QNSdf vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:snapshot role:public active:true size:134217728 origin-name:linear3 origin-uuid:ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:linear3-snap2 lv-uuid:CMRFlu-B5Qf-fSiN-0EmQ-zlgS-M4G1-L8ED0u vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:thin role:public active:true size:1073741824 pool-name:thin-pool pool-uuid:1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5 origin-name:linear3 origin-uuid:ATQF1G-J5Bf-D00G-NNnc-eXhH-ImpX-dHdcCs segments:<> }",
	"lv:{ lv-name:lvol0 lv-uuid:sH9o8O-vxn2-AWn3-x11h-UfJE-TzHv-Vp0ETU vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:normal role:public active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:thin-pool lv-uuid:1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5 vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:thin-pool role:private active:true size:4294967296 data-name:[thin-pool_tdata] data-uuid:GgEqYb-YsrV-HviJ-0SNI-eMDH-q37w-tQRpLh metadata-name:[thin-pool_tmeta] metadata-uuid:2GOZZy-rjkB-A0z1-dMrG-oWpv-eJWH-UlhKPw segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:thin2 lv-uuid:fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:thin role:public active:true size:2147483648 pool-name:thin-pool pool-uuid:1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5 segments:<> }",
	"lv:{ lv-name:thin2-snap1 lv-uuid:3oli42-K3HH-dkl8-RNOh-MEpq-2ZKI-3nO1en vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:thin role:public active:true size:2147483648 pool-name:thin-pool pool-uuid:1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5 origin-name:thin2 origin-uuid:fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf segments:<> }",
	"lv:{ lv-name:thin2-snap2 lv-uuid:e24hyo-e0py-QNdk-xqdR-n2ZX-NBZb-0MkL5B vg-name:test vg-uuid:e9UqnS-Afr3-qwoY-GEbf-aS0N-JXiV-tswJd7 lv-type:thin role:public active:false size:2147483648 pool-name:thin-pool pool-uuid:1alqOV-31y9-rspR-58hl-6HcK-cBtc-gBcTW5 origin-name:thin2 origin-uuid:fA6gcX-bmO4-ZeKX-eASS-SrJs-MsE6-33f0Tf segments:<> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse7)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"mirror\", \"lv_uuid\":\"DT3sif-b6pw-gLME-dsYR-edOs-z5yg-5fLIzW\", \"vg_name\":\"test\", \"vg_uuid\":\"k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8\", \"lv_role\":\"public\", \"lv_attr\":\"mwi-a-m---\", \"lv_size\":\"5368709120\", \"segtype\":\"mirror\", \"stripes\":\"2\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mimage_0]\", \"lv_uuid\":\"vnsHgX-rxIQ-ZUZI-rkAe-nOpi-xBXA-sS0DKG\", \"vg_name\":\"test\", \"vg_uuid\":\"k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8\", \"lv_role\":\"private,mirror,image\", \"lv_attr\":\"iwi-aom---\", \"lv_size\":\"5368709120\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mimage_1]\", \"lv_uuid\":\"8Jhzus-TSBI-tWPy-xXQu-9D9s-947j-EoAtZR\", \"vg_name\":\"test\", \"vg_uuid\":\"k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8\", \"lv_role\":\"private,mirror,image\", \"lv_attr\":\"iwi-aom---\", \"lv_size\":\"5368709120\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mlog]\", \"lv_uuid\":\"2LquQX-Flts-JO5X-wLYS-Q0Q6-m3gB-6WsLD6\", \"vg_name\":\"test\", \"vg_uuid\":\"k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8\", \"lv_role\":\"private,mirror,log\", \"lv_attr\":\"lwi-aom---\", \"lv_size\":\"4194304\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"}"
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:[mirror_mimage_0] lv-uuid:vnsHgX-rxIQ-ZUZI-rkAe-nOpi-xBXA-sS0DKG vg-name:test vg-uuid:k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8 lv-type:unknown role:private active:true size:5368709120 segments:<stripes:1> }",
	"lv:{ lv-name:[mirror_mimage_1] lv-uuid:8Jhzus-TSBI-tWPy-xXQu-9D9s-947j-EoAtZR vg-name:test vg-uuid:k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8 lv-type:unknown role:private active:true size:5368709120 segments:<stripes:1> }",
	"lv:{ lv-name:[mirror_mlog] lv-uuid:2LquQX-Flts-JO5X-wLYS-Q0Q6-m3gB-6WsLD6 vg-name:test vg-uuid:k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8 lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:mirror lv-uuid:DT3sif-b6pw-gLME-dsYR-edOs-z5yg-5fLIzW vg-name:test vg-uuid:k8GRHJ-LwHN-xzmX-kocx-bWOA-tuBz-CD6Fo8 lv-type:mirror role:public active:true size:5368709120 segments:<stripes:2> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse8)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"linear1\", \"lv_uuid\":\"9TJX53-bhjT-mtpw-oAxe-DsOG-XBoh-Z71v3h\", \"vg_name\":\"test\", \"vg_uuid\":\"n3gA8C-3cyA-GE9m-fx7K-zExF-cMD7-nF3x7N\", \"lv_role\":\"public,origin,thickorigin,multithickorigin\", \"lv_attr\":\"owi-a-s---\", \"lv_size\":\"10737418240\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear1-snap1\", \"lv_uuid\":\"GYl8hq-V0Jl-i2ac-T9A5-kVBt-1JCx-ed4g34\", \"vg_name\":\"test\", \"vg_uuid\":\"n3gA8C-3cyA-GE9m-fx7K-zExF-cMD7-nF3x7N\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"linear1\", \"origin_uuid\":\"9TJX53-bhjT-mtpw-oAxe-DsOG-XBoh-Z71v3h\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:linear1 lv-uuid:9TJX53-bhjT-mtpw-oAxe-DsOG-XBoh-Z71v3h vg-name:test vg-uuid:n3gA8C-3cyA-GE9m-fx7K-zExF-cMD7-nF3x7N lv-type:normal role:public active:true size:10737418240 origin-size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:linear1-snap1 lv-uuid:GYl8hq-V0Jl-i2ac-T9A5-kVBt-1JCx-ed4g34 vg-name:test vg-uuid:n3gA8C-3cyA-GE9m-fx7K-zExF-cMD7-nF3x7N lv-type:snapshot role:public active:true size:1073741824 origin-size:10737418240 origin-name:linear1 origin-uuid:9TJX53-bhjT-mtpw-oAxe-DsOG-XBoh-Z71v3h segments:<stripes:1 chunk-size:4096> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse9)
{
    // Several logical volumes with snapshots that are reported as origin by lvs, so the
    // type (normal, raid, cache, ...) depends on the segtype.

    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"cache1\", \"lv_uuid\":\"hOiWKM-Cy45-OTvw-d4vR-inXD-bol2-zw7ses\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-C---\", \"lv_size\":\"10737418240\", \"origin_size\":\"10737418240\", \"segtype\":\"cache\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"[cache1-volume]\", \"pool_lv_uuid\":\"3Roi9q-GIIJ-yFvy-F5fc-wazU-kMdX-4CaVJh\", \"origin\":\"[cache1_corig]\", \"origin_uuid\":\"1Uy23z-t5Re-WfZX-c0B6-jHhR-JuF4-W7JbX4\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache1-snap\", \"lv_uuid\":\"BMgF2x-ft26-EuK4-PfPv-BHEr-MMAP-a9Uscw\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"cache1\", \"origin_uuid\":\"hOiWKM-Cy45-OTvw-d4vR-inXD-bol2-zw7ses\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache1-volume]\", \"lv_uuid\":\"3Roi9q-GIIJ-yFvy-F5fc-wazU-kMdX-4CaVJh\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private\", \"lv_attr\":\"Cwi-aoC---\", \"lv_size\":\"1073741824\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache1_corig]\", \"lv_uuid\":\"1Uy23z-t5Re-WfZX-c0B6-jHhR-JuF4-W7JbX4\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,cache,origin,cacheorigin\", \"lv_attr\":\"owi-aoC---\", \"lv_size\":\"10737418240\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache2\", \"lv_uuid\":\"kyQmHp-8HiZ-PtK0-7TxV-FcVV-iRWi-8nugzx\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-C---\", \"lv_size\":\"10737418240\", \"origin_size\":\"10737418240\", \"segtype\":\"cache\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"[cache2-pool]\", \"pool_lv_uuid\":\"CAFMIn-I6Sf-MecC-Ml9y-8rHu-2e14-jkRfng\", \"origin\":\"[cache2_corig]\", \"origin_uuid\":\"pwFXSG-PJ9U-jTgO-PL3z-7SCZ-hZ0D-RlUFT6\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2-pool]\", \"lv_uuid\":\"CAFMIn-I6Sf-MecC-Ml9y-8rHu-2e14-jkRfng\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private\", \"lv_attr\":\"Cwi---C---\", \"lv_size\":\"1073741824\", \"origin_size\":\"\", \"segtype\":\"cache-pool\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"65536\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"[cache2-pool_cdata]\", \"data_lv_uuid\":\"PyuiGz-CB7m-H81T-w4Am-7bM3-92O1-Ja391A\", \"metadata_lv\":\"[cache2-pool_cmeta]\", \"metadata_lv_uuid\":\"LOIc6h-R7ji-atV0-IcMz-1d0K-1vyw-cIbMj5\"},",
	"                  {\"lv_name\":\"[cache2-pool_cdata]\", \"lv_uuid\":\"PyuiGz-CB7m-H81T-w4Am-7bM3-92O1-Ja391A\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,cache,pool,data\", \"lv_attr\":\"Cwi-ao----\", \"lv_size\":\"1073741824\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2-pool_cmeta]\", \"lv_uuid\":\"LOIc6h-R7ji-atV0-IcMz-1d0K-1vyw-cIbMj5\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,cache,pool,metadata\", \"lv_attr\":\"ewi-ao----\", \"lv_size\":\"8388608\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"cache2-snap\", \"lv_uuid\":\"occp3W-hd5C-4Upb-MJ08-bj0W-6qSK-DEolsv\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"cache2\", \"origin_uuid\":\"kyQmHp-8HiZ-PtK0-7TxV-FcVV-iRWi-8nugzx\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[cache2_corig]\", \"lv_uuid\":\"pwFXSG-PJ9U-jTgO-PL3z-7SCZ-hZ0D-RlUFT6\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,cache,origin,cacheorigin\", \"lv_attr\":\"owi-aoC---\", \"lv_size\":\"10737418240\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear\", \"lv_uuid\":\"Uc5EDE-fVaz-9N6M-rkYh-iq4n-kHZu-3qOjxX\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-s---\", \"lv_size\":\"10737418240\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"linear-snap\", \"lv_uuid\":\"XNxQEY-xgXk-ztle-9vtN-ayma-tWVL-xEDNR5\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"linear\", \"origin_uuid\":\"Uc5EDE-fVaz-9N6M-rkYh-iq4n-kHZu-3qOjxX\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[lvol0_pmspare]\", \"lv_uuid\":\"c9k6dk-qgwV-ZUU1-et7J-jS6b-nKby-THgkio\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,pool,spare\", \"lv_attr\":\"ewi-------\", \"lv_size\":\"8388608\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"mirror\", \"lv_uuid\":\"iW0JIT-d1Dz-DEGS-b1uM-1d7h-gBx7-yLHVY9\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-m---\", \"lv_size\":\"2147483648\", \"origin_size\":\"2147483648\", \"segtype\":\"mirror\", \"stripes\":\"2\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"mirror-snap\", \"lv_uuid\":\"jv03Ag-gctg-E2lF-GtGZ-nNRe-7N3d-ZaEggG\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"2147483648\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"mirror\", \"origin_uuid\":\"iW0JIT-d1Dz-DEGS-b1uM-1d7h-gBx7-yLHVY9\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mimage_0]\", \"lv_uuid\":\"ey2DSj-J6ge-WQvp-MGvP-q6WB-G0ko-zwJb71\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,mirror,image\", \"lv_attr\":\"iwi-aom---\", \"lv_size\":\"2147483648\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mimage_1]\", \"lv_uuid\":\"bl5iiI-iLz0-CgZJ-EoD2-vMvo-brhj-MbKzhq\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,mirror,image\", \"lv_attr\":\"iwi-aom---\", \"lv_size\":\"2147483648\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[mirror_mlog]\", \"lv_uuid\":\"Jgxglt-c14Y-l4cb-xOHI-fT50-Zm9n-cV8urT\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,mirror,log\", \"lv_attr\":\"lwi-aom---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"raid6\", \"lv_uuid\":\"n1rm7u-BtXa-srwK-pUaL-TZvf-9hZr-Jah84M\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-r---\", \"lv_size\":\"2151677952\", \"origin_size\":\"2151677952\", \"segtype\":\"raid6\", \"stripes\":\"5\", \"stripe_size\":\"65536\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"raid6-snap\", \"lv_uuid\":\"ylBCXu-Sm7u-8jbz-qHd8-lVsK-fq1T-ReBZUO\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"2151677952\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"raid6\", \"origin_uuid\":\"n1rm7u-BtXa-srwK-pUaL-TZvf-9hZr-Jah84M\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rimage_0]\", \"lv_uuid\":\"1V6IID-fOXI-KeRv-ovjy-UzAC-wnST-rFv2ln\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,image\", \"lv_attr\":\"iwi-aor---\", \"lv_size\":\"717225984\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rimage_1]\", \"lv_uuid\":\"RWGZnn-g1Ju-iIHm-AwB5-9f8j-Y0Q8-c2TZKW\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,image\", \"lv_attr\":\"iwi-aor---\", \"lv_size\":\"717225984\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rimage_2]\", \"lv_uuid\":\"NfgHNx-k4YK-cW20-MKbK-9iQ3-NchX-psNNVt\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,image\", \"lv_attr\":\"iwi-aor---\", \"lv_size\":\"717225984\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rimage_3]\", \"lv_uuid\":\"pFymCC-OT6c-51wn-CaB7-Qr3h-87Sf-nns82A\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,image\", \"lv_attr\":\"iwi-aor---\", \"lv_size\":\"717225984\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rimage_4]\", \"lv_uuid\":\"S9pGmS-MKxJ-EJg4-1GHj-7Lvy-h7Ij-iCM61C\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,image\", \"lv_attr\":\"iwi-aor---\", \"lv_size\":\"717225984\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rmeta_0]\", \"lv_uuid\":\"TDZuIK-nm3E-N4Ug-VGuW-BVTt-0Q8y-YyZdgw\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,metadata\", \"lv_attr\":\"ewi-aor---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rmeta_1]\", \"lv_uuid\":\"8Ytd8Q-dZEs-8Fzo-TkIY-3w4N-A8SH-FArGcD\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,metadata\", \"lv_attr\":\"ewi-aor---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rmeta_2]\", \"lv_uuid\":\"LcgnBT-Hb5O-JsT9-rJn4-azZa-X9ln-0ZL22x\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,metadata\", \"lv_attr\":\"ewi-aor---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rmeta_3]\", \"lv_uuid\":\"cJGldu-kR6W-jN1t-4RkQ-vzsK-exDX-7XT2Mz\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,metadata\", \"lv_attr\":\"ewi-aor---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"[raid6_rmeta_4]\", \"lv_uuid\":\"9tnCbg-rhqO-P6RU-oycL-Lmw9-MT3z-17DYeU\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"private,raid,metadata\", \"lv_attr\":\"ewi-aor---\", \"lv_size\":\"4194304\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"striped\", \"lv_uuid\":\"wwaG9y-0eO6-YV8V-oodX-esnr-H6ou-14lrvj\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,origin,thickorigin\", \"lv_attr\":\"owi-a-s---\", \"lv_size\":\"10737418240\", \"origin_size\":\"10737418240\", \"segtype\":\"striped\", \"stripes\":\"2\", \"stripe_size\":\"65536\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"},",
	"                  {\"lv_name\":\"striped-snap\", \"lv_uuid\":\"sDdckH-L9qv-3s3B-90fY-236h-pjH5-41Ac07\", \"vg_name\":\"test\", \"vg_uuid\":\"hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz\", \"lv_role\":\"public,snapshot,thicksnapshot\", \"lv_attr\":\"swi-a-s---\", \"lv_size\":\"1073741824\", \"origin_size\":\"10737418240\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"4096\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"striped\", \"origin_uuid\":\"wwaG9y-0eO6-YV8V-oodX-esnr-H6ou-14lrvj\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:[cache1-volume] lv-uuid:3Roi9q-GIIJ-yFvy-F5fc-wazU-kMdX-4CaVJh vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache1_corig] lv-uuid:1Uy23z-t5Re-WfZX-c0B6-jHhR-JuF4-W7JbX4 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool] lv-uuid:CAFMIn-I6Sf-MecC-Ml9y-8rHu-2e14-jkRfng vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:cache-pool role:private active:false size:1073741824 data-name:[cache2-pool_cdata] data-uuid:PyuiGz-CB7m-H81T-w4Am-7bM3-92O1-Ja391A metadata-name:[cache2-pool_cmeta] metadata-uuid:LOIc6h-R7ji-atV0-IcMz-1d0K-1vyw-cIbMj5 segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:[cache2-pool_cdata] lv-uuid:PyuiGz-CB7m-H81T-w4Am-7bM3-92O1-Ja391A vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool_cmeta] lv-uuid:LOIc6h-R7ji-atV0-IcMz-1d0K-1vyw-cIbMj5 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2_corig] lv-uuid:pwFXSG-PJ9U-jTgO-PL3z-7SCZ-hZ0D-RlUFT6 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[lvol0_pmspare] lv-uuid:c9k6dk-qgwV-ZUU1-et7J-jS6b-nKby-THgkio vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[mirror_mimage_0] lv-uuid:ey2DSj-J6ge-WQvp-MGvP-q6WB-G0ko-zwJb71 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:2147483648 segments:<stripes:1> }",
	"lv:{ lv-name:[mirror_mimage_1] lv-uuid:bl5iiI-iLz0-CgZJ-EoD2-vMvo-brhj-MbKzhq vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:2147483648 segments:<stripes:1> }",
	"lv:{ lv-name:[mirror_mlog] lv-uuid:Jgxglt-c14Y-l4cb-xOHI-fT50-Zm9n-cV8urT vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rimage_0] lv-uuid:1V6IID-fOXI-KeRv-ovjy-UzAC-wnST-rFv2ln vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:717225984 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rimage_1] lv-uuid:RWGZnn-g1Ju-iIHm-AwB5-9f8j-Y0Q8-c2TZKW vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:717225984 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rimage_2] lv-uuid:NfgHNx-k4YK-cW20-MKbK-9iQ3-NchX-psNNVt vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:717225984 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rimage_3] lv-uuid:pFymCC-OT6c-51wn-CaB7-Qr3h-87Sf-nns82A vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:717225984 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rimage_4] lv-uuid:S9pGmS-MKxJ-EJg4-1GHj-7Lvy-h7Ij-iCM61C vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:717225984 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rmeta_0] lv-uuid:TDZuIK-nm3E-N4Ug-VGuW-BVTt-0Q8y-YyZdgw vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rmeta_1] lv-uuid:8Ytd8Q-dZEs-8Fzo-TkIY-3w4N-A8SH-FArGcD vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rmeta_2] lv-uuid:LcgnBT-Hb5O-JsT9-rJn4-azZa-X9ln-0ZL22x vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rmeta_3] lv-uuid:cJGldu-kR6W-jN1t-4RkQ-vzsK-exDX-7XT2Mz vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:[raid6_rmeta_4] lv-uuid:9tnCbg-rhqO-P6RU-oycL-Lmw9-MT3z-17DYeU vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:unknown role:private active:true size:4194304 segments:<stripes:1> }",
	"lv:{ lv-name:cache1 lv-uuid:hOiWKM-Cy45-OTvw-d4vR-inXD-bol2-zw7ses vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:cache role:public active:true size:10737418240 origin-size:10737418240 pool-name:[cache1-volume] pool-uuid:3Roi9q-GIIJ-yFvy-F5fc-wazU-kMdX-4CaVJh origin-name:[cache1_corig] origin-uuid:1Uy23z-t5Re-WfZX-c0B6-jHhR-JuF4-W7JbX4 segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache1-snap lv-uuid:BMgF2x-ft26-EuK4-PfPv-BHEr-MMAP-a9Uscw vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:10737418240 origin-name:cache1 origin-uuid:hOiWKM-Cy45-OTvw-d4vR-inXD-bol2-zw7ses segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:cache2 lv-uuid:kyQmHp-8HiZ-PtK0-7TxV-FcVV-iRWi-8nugzx vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:cache role:public active:true size:10737418240 origin-size:10737418240 pool-name:[cache2-pool] pool-uuid:CAFMIn-I6Sf-MecC-Ml9y-8rHu-2e14-jkRfng origin-name:[cache2_corig] origin-uuid:pwFXSG-PJ9U-jTgO-PL3z-7SCZ-hZ0D-RlUFT6 segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache2-snap lv-uuid:occp3W-hd5C-4Upb-MJ08-bj0W-6qSK-DEolsv vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:10737418240 origin-name:cache2 origin-uuid:kyQmHp-8HiZ-PtK0-7TxV-FcVV-iRWi-8nugzx segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:linear lv-uuid:Uc5EDE-fVaz-9N6M-rkYh-iq4n-kHZu-3qOjxX vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:public active:true size:10737418240 origin-size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:linear-snap lv-uuid:XNxQEY-xgXk-ztle-9vtN-ayma-tWVL-xEDNR5 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:10737418240 origin-name:linear origin-uuid:Uc5EDE-fVaz-9N6M-rkYh-iq4n-kHZu-3qOjxX segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:mirror lv-uuid:iW0JIT-d1Dz-DEGS-b1uM-1d7h-gBx7-yLHVY9 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:mirror role:public active:true size:2147483648 origin-size:2147483648 segments:<stripes:2> }",
	"lv:{ lv-name:mirror-snap lv-uuid:jv03Ag-gctg-E2lF-GtGZ-nNRe-7N3d-ZaEggG vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:2147483648 origin-name:mirror origin-uuid:iW0JIT-d1Dz-DEGS-b1uM-1d7h-gBx7-yLHVY9 segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:raid6 lv-uuid:n1rm7u-BtXa-srwK-pUaL-TZvf-9hZr-Jah84M vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:raid role:public active:true size:2151677952 origin-size:2151677952 segments:<stripes:5 stripe-size:65536> }",
	"lv:{ lv-name:raid6-snap lv-uuid:ylBCXu-Sm7u-8jbz-qHd8-lVsK-fq1T-ReBZUO vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:2151677952 origin-name:raid6 origin-uuid:n1rm7u-BtXa-srwK-pUaL-TZvf-9hZr-Jah84M segments:<stripes:1 chunk-size:4096> }",
	"lv:{ lv-name:striped lv-uuid:wwaG9y-0eO6-YV8V-oodX-esnr-H6ou-14lrvj vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:normal role:public active:true size:10737418240 origin-size:10737418240 segments:<stripes:2 stripe-size:65536> }",
	"lv:{ lv-name:striped-snap lv-uuid:sDdckH-L9qv-3s3B-90fY-236h-pjH5-41Ac07 vg-name:test vg-uuid:hW15Gj-YaBO-Y7yK-Xdc0-L04A-UyJk-2cg3Wz lv-type:snapshot role:public active:true size:1073741824 origin-size:10737418240 origin-name:striped origin-uuid:wwaG9y-0eO6-YV8V-oodX-esnr-H6ou-14lrvj segments:<stripes:1 chunk-size:4096> }"
    };

    check(input, output);
}


BOOST_AUTO_TEST_CASE(parse10)
{
    vector<string> input = {
	"  {",
	"      \"report\": [",
	"          {",
	"              \"lv\": [",
	"                  {\"lv_name\":\"c\", \"lv_uuid\":\"gqnD48-ev4p-nCdM-EWEP-RAgW-TaDC-h9Hb13\", \"vg_name\":\"b\", \"vg_uuid\":\"GbcwxM-Px5E-Xs7u-dLhx-r7RU-4LG4-uGR6Ew\", \"lv_role\":\"public\", \"lv_attr\":\"-wi-a-----\", \"lv_size\":\"107369988096\", \"origin_size\":\"\", \"segtype\":\"linear\", \"stripes\":\"1\", \"stripe_size\":\"0\", \"chunk_size\":\"0\", \"pool_lv\":\"\", \"pool_lv_uuid\":\"\", \"origin\":\"\", \"origin_uuid\":\"\", \"data_lv\":\"\", \"data_lv_uuid\":\"\", \"metadata_lv\":\"\", \"metadata_lv_uuid\":\"\"}",
	"              ]",
	"          }",
	"      ]",
	"  }"
    };

    vector<string> output = {
	"lv:{ lv-name:c lv-uuid:gqnD48-ev4p-nCdM-EWEP-RAgW-TaDC-h9Hb13 vg-name:b vg-uuid:GbcwxM-Px5E-Xs7u-dLhx-r7RU-4LG4-uGR6Ew lv-type:normal role:public active:true size:107369988096 segments:<stripes:1> }"
    };

    check("b", "c", input, output);
}

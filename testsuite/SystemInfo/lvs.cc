
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
    Mockup::set_command(LVS_BIN " --reportformat json --units b --nosuffix --all --options lv_name,"
			"lv_uuid,vg_name,vg_uuid,lv_role,lv_attr,lv_size,segtype,stripes,stripe_size,"
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
	"lv:{ lv-name:[cache1-volume] lv-uuid:su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache1_corig] lv-uuid:ZzFhN5-b2N6-dL4F-H42U-dAP9-laF2-EU1F3p vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool] lv-uuid:DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache-pool role:private active:false size:1073741824 data-name:[cache2-pool_cdata] data-uuid:Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q metadata-name:[cache2-pool_cmeta] metadata-uuid:3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:[cache2-pool_cdata] lv-uuid:Rkyj1k-9Ahg-mBQP-n820-bKvE-ZAzU-fwXJ6Q vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2-pool_cmeta] lv-uuid:3b8V9E-3Scj-Ox8j-6pss-QZIM-80jx-hDXYEi vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[cache2_corig] lv-uuid:qhafQi-Nznt-WJCP-bMpl-Hx1x-w8tb-kt0EEa vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:[cache3-pool_cdata] lv-uuid:BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:false size:1073741824 segments:<stripes:1> }",
	"lv:{ lv-name:[cache3-pool_cmeta] lv-uuid:fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:[lvol0_pmspare] lv-uuid:vLO1sd-Slsj-VY6P-sTzJ-ZEya-0hFZ-uAJTuq vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:unknown role:private active:false size:8388608 segments:<stripes:1> }",
	"lv:{ lv-name:cache1 lv-uuid:R3wAkS-5BJp-VwEe-DS9r-gBJG-jlgB-MvwUGG vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache role:public active:true size:10737418240 pool-name:[cache1-volume] pool-uuid:su0ZFA-ejBI-M3VL-gQDD-UNcu-W0o8-2aBdtj segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache2 lv-uuid:N123dc-RYSs-YEAG-KcLk-kgki-Dvlx-Iwgj3L vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache role:public active:true size:10737418240 pool-name:[cache2-pool] pool-uuid:DfS7Ct-j41n-oz2e-C8vE-RuSt-blac-NjJwkW segments:<stripes:1 chunk-size:65536> }",
	"lv:{ lv-name:cache3 lv-uuid:e0Lwjb-3rMk-RwYB-OWYf-YInP-H1UJ-1HpYtJ vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:normal role:public active:true size:10737418240 segments:<stripes:1> }",
	"lv:{ lv-name:cache3-pool lv-uuid:vrpci4-gT93-VZZg-Z1DU-YvOr-9E7e-TzXmgO vg-name:test vg-uuid:55auVT-aQ8G-MPiA-uXy1-dvJa-XNOs-6BWsXC lv-type:cache-pool role:private active:false size:1073741824 data-name:[cache3-pool_cdata] data-uuid:BnZkZ2-gkl8-7nL8-dsai-meu0-vhht-OYteZH metadata-name:[cache3-pool_cmeta] metadata-uuid:fSwqC3-OCBQ-UYSF-hV5Z-6NRB-TQL0-y3wYzS segments:<stripes:1 chunk-size:65536> }"
    };

    check(input, output);
}

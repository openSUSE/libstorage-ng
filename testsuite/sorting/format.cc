
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(format1)
{
    static const vector<NameSchema> name_schemata = {
	NameSchema(regex("/dev/sd([a-z]+)", regex::extended), { { PadInfo::A1, 5 } }),
	NameSchema(regex("/dev/md([0-9]+)", regex::extended), { { PadInfo::N1, 3 } }),
	NameSchema(regex("/dev/nvme([0-9]+)n([0-9]+)", regex::extended), { { PadInfo::N1, 3 }, { PadInfo::N1, 3 } }),
    };

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/sda", name_schemata), "/dev/sd00001");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/sdaa", name_schemata), "/dev/sd00027");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/sdaaa", name_schemata), "/dev/sd00703");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md", name_schemata), "/dev/md");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md1", name_schemata), "/dev/md001");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md12", name_schemata), "/dev/md012");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md123", name_schemata), "/dev/md123");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md1234", name_schemata), "/dev/md1234");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/nvme1n2", name_schemata), "/dev/nvme001n002");
}

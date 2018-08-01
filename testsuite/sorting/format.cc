
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/AppUtil.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(format)
{
    static const vector<NameSchema> name_schemata = {
	NameSchema(regex("/dev/sd([a-z]+)", regex::extended), { { 4, ' ' } }),
	NameSchema(regex("/dev/md([0-9]+)", regex::extended), { { 3, '0' } }),
	NameSchema(regex("/dev/nvme([0-9]+)n([0-9]+)", regex::extended), { { 3, '0' }, { 3, '0' } }),
	NameSchema(regex("/dev/xvd([a-z]+)([0-9]+)", regex::extended), { { 4, ' ' }, { 3, '0' } }),
    };

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/sda", name_schemata), "/dev/sd   a");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md", name_schemata), "/dev/md");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md1", name_schemata), "/dev/md001");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md12", name_schemata), "/dev/md012");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md123", name_schemata), "/dev/md123");
    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/md1234", name_schemata), "/dev/md1234");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/nvme1n2", name_schemata), "/dev/nvme001n002");

    BOOST_CHECK_EQUAL(format_to_name_schemata("/dev/xvda1", name_schemata), "/dev/xvd   a001");
}

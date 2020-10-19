
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


// Check that mdadm is called with the correct parameters, esp. the
// correct ordering of devices.

BOOST_AUTO_TEST_CASE(actions)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("create1", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_EQUAL(required_features(cmp.get_probed()), "");
    BOOST_CHECK_EQUAL(suggested_features(cmp.get_probed()), "");

    BOOST_CHECK_EQUAL(required_features(cmp.get_staging()), "md-raid");
    BOOST_CHECK_EQUAL(suggested_features(cmp.get_staging()), "md-raid");

    BOOST_CHECK_EQUAL(features(cmp.get_actiongraph()), "md-raid");
}

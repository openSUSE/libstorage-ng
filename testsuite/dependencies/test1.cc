
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("test1");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_EQUAL(required_features(cmp.get_probed()), "");
    BOOST_CHECK_EQUAL(suggested_features(cmp.get_probed()), "");

    BOOST_CHECK_EQUAL(required_features(cmp.get_staging()), "");
    BOOST_CHECK_EQUAL(suggested_features(cmp.get_staging()), "ext4");

    BOOST_CHECK_EQUAL(features(cmp.get_actiongraph()), "ext4");
}

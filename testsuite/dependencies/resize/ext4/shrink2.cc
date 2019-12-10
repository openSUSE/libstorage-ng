
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


// Check shrinking the partition of an ext4 while also being renamed.

BOOST_AUTO_TEST_CASE(actions)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("shrink2", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

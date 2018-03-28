
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // Check that filesystems on a grown blk device are deleted before and
    // created after the blk device is resized.

    TsCmpActiongraph cmp("resize5");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

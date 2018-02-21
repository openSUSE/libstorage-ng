
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // An example including renaming of logical partitions and grow and shrink
    // actions on resized partitions.

    TsCmpActiongraph cmp("resize3");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

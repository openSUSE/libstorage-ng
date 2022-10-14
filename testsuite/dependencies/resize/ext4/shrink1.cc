
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


// Check shrinking the partition of an ext4.

BOOST_AUTO_TEST_CASE(actions)
{
    setenv("LIBSTORAGE_OS_FLAVOUR", "suse", 1);

    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("shrink1", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

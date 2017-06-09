
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // An example where two fstab entries must be renamed in the correct order.

    TsCmpActiongraph cmp("rename3");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

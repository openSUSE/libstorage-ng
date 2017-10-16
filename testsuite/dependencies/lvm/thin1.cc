
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // Action of thins in a thin-pool almost need no dependencies due to thin
    // provisioning and overcommitting. Only deleting has to happen before
    // creating since the names might collide (even across thin pools in one
    // volume group).

    TsCmpActiongraph cmp("thin1");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

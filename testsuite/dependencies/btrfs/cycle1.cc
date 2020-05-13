
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


/**
 * Delete two subvolumes with a cycle. The Snapshot holder is ignored when calculating the
 * actiongraph by using View::REMOVE. The setup might not have any practical use-case.
 */
BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("cycle1");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

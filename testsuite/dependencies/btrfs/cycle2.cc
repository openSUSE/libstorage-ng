
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


/**
 * When trying to create two btrfs subvolumes with a cycle an exception is thrown. The
 * setup might not have any practical use-case.
 *
 * The calculation of the actiongraph could ignore the problematic holders since so far
 * snapshots cannot be created at all (are created as plain subvolumes). But in case
 * snapshots should ever be created properly the problem would raise again. So better make
 * is clear from the beginning that creating cycles is not supported.
 */
BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    BOOST_CHECK_EXCEPTION(TsCmpActiongraph cmp("cycle2"), Exception,
       [](const Exception& e) { return e.what() == "actiongraph not a DAG"s; }
    );
}

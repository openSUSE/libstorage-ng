
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;

/**
 * The lvremove command will remove snapshots of linear logical volumes when removing the
 * origin. So libstorage-ng has to remove the snapshots before the origin since otherwise
 * removing the snapshots fails (as they were already removed by lvremove). This does not
 * apply to snapshots of thin logical volumes.
 */
BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("snapshot1");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

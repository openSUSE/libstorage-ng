
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // TODO the dependency between the two mount actions is not needed

    TsCmpActiongraph cmp("replace1");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

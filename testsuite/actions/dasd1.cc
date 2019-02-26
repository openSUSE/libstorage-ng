
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


// Check that the sector numbers are adjusted for DASDs named
// /dev/dasd* but not for DASDs named /dev/vd*.

BOOST_AUTO_TEST_CASE(actions)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("dasd1", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

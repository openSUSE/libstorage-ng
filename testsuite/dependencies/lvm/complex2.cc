
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    // A complex example including
    //  - Shrinking and creating LVs in a VG without modifying its PVs.
    //  - Full creation of a second independent VG in the same system
    //    (from partitions to LVs).
    TsCmpActiongraph cmp("complex2");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

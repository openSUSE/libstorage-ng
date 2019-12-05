
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
    //  - Extending and reducing a VG
    //  - Growing and shrinking volumes within that VG
    //  - Adding a new LV to the VG
    TsCmpActiongraph cmp("complex1", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

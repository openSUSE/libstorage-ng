
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(multidevice_btrfs_dependencies)
{
    set_logger(get_stdout_logger());

    // Check that a multi-device btrfs is properly grown after resizing
    // one of its devices

    TsCmpActiongraph cmp("resize6");
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/Logger.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    setenv("LIBSTORAGE_OS_FLAVOUR", "suse", 1);

    set_logger(get_stdout_logger());

    TsCmpActiongraph cmp("repair1", true);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

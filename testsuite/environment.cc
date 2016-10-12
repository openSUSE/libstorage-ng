
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(copy)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    environment.set_devicegraph_filename("test1.xml");

    Storage storage(environment);

    environment.set_devicegraph_filename("test2.xml");

    BOOST_CHECK_EQUAL(environment.get_devicegraph_filename(), "test2.xml");
    BOOST_CHECK_EQUAL(storage.get_environment().get_devicegraph_filename(), "test1.xml");
}

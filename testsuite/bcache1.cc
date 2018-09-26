
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Bcache.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(free_name)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Bcache::create(staging, "/dev/bcache0");

    BOOST_CHECK_EQUAL(Bcache::find_free_name(staging), "/dev/bcache1");

    Bcache::create(staging, "/dev/bcache1");

    BOOST_CHECK_EQUAL(Bcache::find_free_name(staging), "/dev/bcache2");
}

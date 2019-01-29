
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

    Bcache::create(staging, "/dev/bcache1", BcacheType::FLASH_ONLY);

    BOOST_CHECK_EQUAL(Bcache::find_free_name(staging), "/dev/bcache2");
}

BOOST_AUTO_TEST_CASE(reassign_numbers)
{
    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("probe/bcache1-devicegraph.xml");

    Storage storage(environment);
    storage.probe();
    storage.check();

    // we need some bcaches on system, here just ensure that no unnoticed xml change happen.
    BOOST_CHECK_EQUAL(Bcache::get_all(storage.get_probed()).size(), 3);

    Devicegraph* staging = storage.get_staging();

    const Bcache* new_bcache1 = Bcache::create(staging, "/dev/bcache3");
    const Bcache* new_bcache2 = Bcache::create(staging, "/dev/bcache4");

    // a bit hidden, but remove_device does call reassign_numbers,
    // so we are sure that after removal of bcache, they have correct numbers.
    staging->remove_device(Bcache::find_by_name(staging, "/dev/bcache1"));

    BOOST_CHECK_EQUAL(Bcache::get_all(staging).size(), 4);
    BOOST_CHECK_EQUAL(new_bcache1->get_name(), "/dev/bcache1");
    BOOST_CHECK_EQUAL(new_bcache2->get_name(), "/dev/bcache3");
}

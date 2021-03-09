
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/Tmpfs.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(copy_individual)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Tmpfs::create(storage.get_staging());

    BOOST_CHECK_EQUAL(storage.get_staging()->num_devices(), 1);

    storage.copy_devicegraph("staging", "backup");

    Tmpfs::create(storage.get_staging());

    BOOST_CHECK_EQUAL(storage.get_staging()->num_devices(), 2);

    storage.restore_devicegraph("backup");

    BOOST_CHECK_EQUAL(storage.get_staging()->num_devices(), 1);

    storage.check();
}

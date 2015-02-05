
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_set_region)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", PRIMARY);

    // setting the region affects the size

    sda1->set_region(Region(0, 1000));

    BOOST_CHECK_EQUAL(sda1->get_region().get_start(), 0);
    BOOST_CHECK_EQUAL(sda1->get_region().get_length(), 1000);

    BOOST_CHECK_EQUAL(sda1->get_size_k(), 8032500);
}


BOOST_AUTO_TEST_CASE(test_set_size_k)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", PRIMARY);

    // setting the size affects the region

    sda1->set_size_k(8032500);

    BOOST_CHECK_EQUAL(sda1->get_size_k(), 8032500);

    BOOST_CHECK_EQUAL(sda1->get_region().get_start(), 0);
    BOOST_CHECK_EQUAL(sda1->get_region().get_length(), 1000);
}

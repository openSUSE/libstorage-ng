
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_set_region)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_region(Region(0, 100000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    // creating a partition also sets the size

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(1, 2000, 512), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(sda1->get_region().get_start(), 1);
    BOOST_CHECK_EQUAL(sda1->get_region().get_length(), 2000);
    BOOST_CHECK_EQUAL(sda1->get_region().get_block_size(), 512);

    BOOST_CHECK_EQUAL(sda1->get_size(), 2000ULL * 512);

    // setting the region affects the size

    sda1->set_region(Region(1, 1000, 512));

    BOOST_CHECK_EQUAL(sda1->get_region().get_start(), 1);
    BOOST_CHECK_EQUAL(sda1->get_region().get_length(), 1000);
    BOOST_CHECK_EQUAL(sda1->get_region().get_block_size(), 512);

    BOOST_CHECK_EQUAL(sda1->get_size(), 1000ULL * 512);
}


BOOST_AUTO_TEST_CASE(test_set_size)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_region(Region(0, 100000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(1, 0, 512), PartitionType::PRIMARY);

    // setting the size affects the region

    sda1->set_size(1000ULL * 512);

    BOOST_CHECK_EQUAL(sda1->get_size(), 500 * KiB);

    BOOST_CHECK_EQUAL(sda1->get_region().get_start(), 1);
    BOOST_CHECK_EQUAL(sda1->get_region().get_length(), 1000);
    BOOST_CHECK_EQUAL(sda1->get_region().get_block_size(), 512);
}

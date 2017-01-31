
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_delete_logical)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(1 * 2048, 100 * 2048, 512), PartitionType::EXTENDED);

    Partition* sda5 = msdos->create_partition("/dev/sda5", Region(2 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);
    msdos->create_partition("/dev/sda6", Region(12 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);
    Partition* sda7 = msdos->create_partition("/dev/sda7", Region(24 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);
    msdos->create_partition("/dev/sda8", Region(36 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);

    msdos->delete_partition(sda5);
    msdos->delete_partition(sda7);

    vector<Partition*> partitions = msdos->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 3);

    // originally sda6
    BOOST_CHECK_EQUAL(partitions[1]->get_name(), "/dev/sda5");
    BOOST_CHECK_EQUAL(partitions[1]->get_region(), Region(12 * 2048, 10 * 2048, 512));

    // originally sda8
    BOOST_CHECK_EQUAL(partitions[2]->get_name(), "/dev/sda6");
    BOOST_CHECK_EQUAL(partitions[2]->get_region(), Region(36 * 2048, 10 * 2048, 512));
}

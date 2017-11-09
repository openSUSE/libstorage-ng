
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_msdos1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);
    msdos->create_partition("/dev/sda2", Region(20 * 2048, 30 * 2048, 512), PartitionType::EXTENDED);
    msdos->create_partition("/dev/sda5", Region(21 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);
    msdos->create_partition("/dev/sda6", Region(31 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);

    vector<Partition*> partitions = msdos->get_partitions();
    sort(partitions.begin(), partitions.end(), Partition::compare_by_number);

    BOOST_REQUIRE_EQUAL(partitions.size(), 4);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/sda1");
    BOOST_CHECK_EQUAL(partitions[1]->get_name(), "/dev/sda2");
    BOOST_CHECK_EQUAL(partitions[2]->get_name(), "/dev/sda5");
    BOOST_CHECK_EQUAL(partitions[3]->get_name(), "/dev/sda6");

    BOOST_CHECK_EQUAL(msdos->num_primary(), 1);
    BOOST_CHECK(msdos->has_extended());
    BOOST_CHECK_EQUAL(msdos->num_logical(), 2);
}

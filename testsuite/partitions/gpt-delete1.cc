
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/GptImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_delete)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 5860466688, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/sda2", Region(2099200, 2097152, 512), PartitionType::PRIMARY);

    storage.remove_devicegraph("probed");
    storage.copy_devicegraph("staging", "probed");

    staging = storage.get_staging();
    sda = Disk::find_by_name(staging, "/dev/sda");
    gpt = sda->get_partition_table();

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 2097152, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/sda3", Region(4196352, 2097152, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/sda4", Region(6293504, 2097152, 512), PartitionType::PRIMARY);

    gpt->delete_partition(sda1);

    storage.check();

    vector<Partition*> partitions = gpt->get_partitions();
    sort(partitions.begin(), partitions.end(), Partition::compare_by_number);

    BOOST_REQUIRE_EQUAL(partitions.size(), 3);

    // originally sda3
    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/sda1");
    BOOST_CHECK_EQUAL(partitions[0]->get_region(), Region(4196352, 2097152, 512));

    // still sda2
    BOOST_CHECK_EQUAL(partitions[1]->get_name(), "/dev/sda2");
    BOOST_CHECK_EQUAL(partitions[1]->get_region(), Region(2099200, 2097152, 512));

    // originally sda4
    BOOST_CHECK_EQUAL(partitions[2]->get_name(), "/dev/sda3");
    BOOST_CHECK_EQUAL(partitions[2]->get_region(), Region(6293504, 2097152, 512));
}

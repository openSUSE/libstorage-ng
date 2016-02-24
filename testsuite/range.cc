
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


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_msdos)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(0, 0, 262144), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(msdos->get_partitionable()->get_range(), 256);

    BOOST_CHECK_EQUAL(msdos->max_primary(), 4);
    BOOST_CHECK_EQUAL(msdos->extended_possible(), true);
    BOOST_CHECK_EQUAL(msdos->max_logical(), 256);

    BOOST_CHECK_EQUAL(msdos->num_primary(), 1);
    BOOST_CHECK_EQUAL(msdos->has_extended(), false);
    BOOST_CHECK_EQUAL(msdos->num_logical(), 0);
}


BOOST_AUTO_TEST_CASE(test_gpt)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/sda1", Region(0, 0, 262144), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(gpt->get_partitionable()->get_range(), 256);

    BOOST_CHECK_EQUAL(gpt->max_primary(), 128);
    BOOST_CHECK_EQUAL(gpt->extended_possible(), false);
    BOOST_CHECK_EQUAL(gpt->max_logical(), 0);

    BOOST_CHECK_EQUAL(gpt->num_primary(), 1);
    BOOST_CHECK_EQUAL(gpt->has_extended(), false);
    BOOST_CHECK_EQUAL(gpt->num_logical(), 0);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
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

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 2048);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 1000000 - 2048);
    BOOST_CHECK_EQUAL(slots[0].nr, 1);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda1");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, true);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_msdos2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);
    msdos->create_partition("/dev/sda2", Region(20 * 2048, 10 * 2048, 512), PartitionType::EXTENDED);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 3);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 11 * 2048);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 9 * 2048);
    BOOST_CHECK_EQUAL(slots[0].nr, 3);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);

    BOOST_CHECK_EQUAL(slots[1].region.get_start(), 30 * 2048);
    BOOST_CHECK_EQUAL(slots[1].region.get_length(), 1000000 - 30 * 2048);
    BOOST_CHECK_EQUAL(slots[1].nr, 3);
    BOOST_CHECK_EQUAL(slots[1].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[1].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[1].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[1].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[1].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[1].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[1].logical_possible, false);

    BOOST_CHECK_EQUAL(slots[2].region.get_start(), 21 * 2048);
    BOOST_CHECK_EQUAL(slots[2].region.get_length(), 9 * 2048);
    BOOST_CHECK_EQUAL(slots[2].nr, 5);
    BOOST_CHECK_EQUAL(slots[2].name, "/dev/sda5");
    BOOST_CHECK_EQUAL(slots[2].primary_slot, false);
    BOOST_CHECK_EQUAL(slots[2].primary_possible, false);
    BOOST_CHECK_EQUAL(slots[2].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[2].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[2].logical_slot, true);
    BOOST_CHECK_EQUAL(slots[2].logical_possible, true);
}


BOOST_AUTO_TEST_CASE(test_msdos3)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    // the disk is bigger than the address space of a MSDOS partition table with 512
    // bytes per sector

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 20000000, 4096));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 256);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 20000000 - 256);
    BOOST_CHECK_EQUAL(slots[0].nr, 1);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda1");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, true);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_gpt1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/sda1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/sda2", Region(20 * 2048, 30 * 2048, 512), PartitionType::PRIMARY);

    vector<PartitionSlot> slots = gpt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 2);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 11 * 2048);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 9 * 2048);
    BOOST_CHECK_EQUAL(slots[0].nr, 3);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);

    // 33 sectors at the end are used for the secondary gpt header

    BOOST_CHECK_EQUAL(slots[1].region.get_start(), 50 * 2048);
    BOOST_CHECK_EQUAL(slots[1].region.get_length(), 1000000 - 50 * 2048 - 33);
    BOOST_CHECK_EQUAL(slots[1].nr, 3);
    BOOST_CHECK_EQUAL(slots[1].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[1].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[1].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[1].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[1].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[1].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[1].logical_possible, false);
}

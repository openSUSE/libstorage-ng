
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


BOOST_AUTO_TEST_CASE(test_msdos1)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    list<PartitionSlotInfo> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    list<PartitionSlotInfo>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->cylRegion.start, 0);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 9999);
    BOOST_CHECK_EQUAL(it->nr, 1);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda1");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, true);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);
}


BOOST_AUTO_TEST_CASE(test_msdos2)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    // primary partition from 0 to 1000
    Partition* sda1 = msdos->create_partition("/dev/sda1", PRIMARY);
    sda1->set_region(Region(0, 1000));

    // extended partition from 2000 to 5000
    Partition* sda2 = msdos->create_partition("/dev/sda2", EXTENDED);
    sda2->set_region(Region(2000, 3000));

    list<PartitionSlotInfo> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 3);

    // primary possible from 1000 to 2000
    list<PartitionSlotInfo>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->cylRegion.start, 1000);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 1000);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, false);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);

    // primary possible from 5000 to 9999
    ++it;
    BOOST_CHECK_EQUAL(it->cylRegion.start, 5000);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 4999);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, false);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);

    // logical possible from 2000 to 4999
    ++it;
    BOOST_CHECK_EQUAL(it->cylRegion.start, 2000);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 2999);
    BOOST_CHECK_EQUAL(it->nr, 5);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda5");
    BOOST_CHECK_EQUAL(it->primarySlot, false);
    BOOST_CHECK_EQUAL(it->primaryPossible, false);
    BOOST_CHECK_EQUAL(it->extendedSlot, false);
    BOOST_CHECK_EQUAL(it->extendedPossible, false);
    BOOST_CHECK_EQUAL(it->logicalSlot, true);
    BOOST_CHECK_EQUAL(it->logicalPossible, true);
}


BOOST_AUTO_TEST_CASE(test_msdos3)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    // the disk is bigger than the address space of a MSDOS partition table with 512
    // bytes per sector

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(534698, 255, 63, 4096));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    list<PartitionSlotInfo> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    list<PartitionSlotInfo>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->cylRegion.start, 0);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 267350);
    BOOST_CHECK_EQUAL(it->nr, 1);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda1");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, true);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);
}


BOOST_AUTO_TEST_CASE(test_gpt1)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_range(256);
    sda->get_impl().set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    // primary partition from 0 to 1000
    Partition* sda1 = gpt->create_partition("/dev/sda1", PRIMARY);
    sda1->set_region(Region(0, 1000));

    // primary partition from 2000 to 5000
    Partition* sda2 = gpt->create_partition("/dev/sda2", PRIMARY);
    sda2->set_region(Region(2000, 3000));

    list<PartitionSlotInfo> slots = gpt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 2);

    // primary possible from 1000 to 2000
    list<PartitionSlotInfo>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->cylRegion.start, 1000);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 1000);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, false);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);

    // primary possible from 5000 to 9999
    ++it;
    BOOST_CHECK_EQUAL(it->cylRegion.start, 5000);
    BOOST_CHECK_EQUAL(it->cylRegion.len, 4999);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->device, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primarySlot, true);
    BOOST_CHECK_EQUAL(it->primaryPossible, true);
    BOOST_CHECK_EQUAL(it->extendedSlot, true);
    BOOST_CHECK_EQUAL(it->extendedPossible, false);
    BOOST_CHECK_EQUAL(it->logicalSlot, false);
    BOOST_CHECK_EQUAL(it->logicalPossible, false);
}

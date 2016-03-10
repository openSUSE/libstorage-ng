
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
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    vector<PartitionSlot>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->region.get_start(), 0);
    BOOST_CHECK_EQUAL(it->region.get_length(), 9999);
    BOOST_CHECK_EQUAL(it->nr, 1);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda1");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, true);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_msdos2)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    // primary partition from 0 to 1000
    msdos->create_partition("/dev/sda1", Region(0, 1000, 8225280), PartitionType::PRIMARY);

    // extended partition from 2000 to 5000
    msdos->create_partition("/dev/sda2", Region(2000, 3000, 8225280), PartitionType::EXTENDED);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 3);

    // primary possible from 1000 to 2000
    vector<PartitionSlot>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->region.get_start(), 1000);
    BOOST_CHECK_EQUAL(it->region.get_length(), 1000);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, false);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);

    // primary possible from 5000 to 9999
    ++it;
    BOOST_CHECK_EQUAL(it->region.get_start(), 5000);
    BOOST_CHECK_EQUAL(it->region.get_length(), 4999);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, false);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);

    // logical possible from 2000 to 4999
    ++it;
    BOOST_CHECK_EQUAL(it->region.get_start(), 2000);
    BOOST_CHECK_EQUAL(it->region.get_length(), 2999);
    BOOST_CHECK_EQUAL(it->nr, 5);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda5");
    BOOST_CHECK_EQUAL(it->primary_slot, false);
    BOOST_CHECK_EQUAL(it->primary_possible, false);
    BOOST_CHECK_EQUAL(it->extended_slot, false);
    BOOST_CHECK_EQUAL(it->extended_possible, false);
    BOOST_CHECK_EQUAL(it->logical_slot, true);
    BOOST_CHECK_EQUAL(it->logical_possible, true);
}


BOOST_AUTO_TEST_CASE(test_msdos3)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    // the disk is bigger than the address space of a MSDOS partition table with 512
    // bytes per sector

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_geometry(Geometry(534698, 255, 63, 4096));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    vector<PartitionSlot>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->region.get_start(), 0);
    BOOST_CHECK_EQUAL(it->region.get_length(), 267350);
    BOOST_CHECK_EQUAL(it->nr, 1);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda1");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, true);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_gpt1)
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->set_range(256);
    sda->set_geometry(Geometry(9999, 255, 63, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    // primary partition from 0 to 1000
    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(0, 1000, 8225280), PartitionType::PRIMARY);
    sda1->set_region(Region(0, 1000, 8225280));

    // primary partition from 2000 to 5000
    Partition* sda2 = gpt->create_partition("/dev/sda2", Region(2000, 3000, 8225280), PartitionType::PRIMARY);
    sda2->set_region(Region(2000, 3000, 8225280));

    vector<PartitionSlot> slots = gpt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 2);

    // primary possible from 1000 to 2000
    vector<PartitionSlot>::const_iterator it = slots.begin();
    BOOST_CHECK_EQUAL(it->region.get_start(), 1000);
    BOOST_CHECK_EQUAL(it->region.get_length(), 1000);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, false);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);

    // primary possible from 5000 to 9999
    ++it;
    BOOST_CHECK_EQUAL(it->region.get_start(), 5000);
    BOOST_CHECK_EQUAL(it->region.get_length(), 4999);
    BOOST_CHECK_EQUAL(it->nr, 3);
    BOOST_CHECK_EQUAL(it->name, "/dev/sda3");
    BOOST_CHECK_EQUAL(it->primary_slot, true);
    BOOST_CHECK_EQUAL(it->primary_possible, true);
    BOOST_CHECK_EQUAL(it->extended_slot, true);
    BOOST_CHECK_EQUAL(it->extended_possible, false);
    BOOST_CHECK_EQUAL(it->logical_slot, false);
    BOOST_CHECK_EQUAL(it->logical_possible, false);
}

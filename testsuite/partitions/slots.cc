
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_msdos1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    BOOST_CHECK_EQUAL(sda->get_range(), 256);

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    BOOST_CHECK_EQUAL(msdos->max_primary(), 4);
    BOOST_CHECK(msdos->extended_possible());
    BOOST_CHECK_EQUAL(msdos->max_logical(), 255);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 2048);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 1000000 - 2048);
    BOOST_CHECK_EQUAL(slots[0].number, 1);
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
    BOOST_CHECK_EQUAL(slots[0].number, 3);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
    BOOST_CHECK(slots[0].is_possible(PartitionType::PRIMARY));
    BOOST_CHECK(!slots[0].is_possible(PartitionType::EXTENDED));
    BOOST_CHECK(!slots[0].is_possible(PartitionType::LOGICAL));

    BOOST_CHECK_EQUAL(slots[1].region.get_start(), 30 * 2048);
    BOOST_CHECK_EQUAL(slots[1].region.get_length(), 1000000 - 30 * 2048);
    BOOST_CHECK_EQUAL(slots[1].number, 3);
    BOOST_CHECK_EQUAL(slots[1].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[1].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[1].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[1].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[1].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[1].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[1].logical_possible, false);
    BOOST_CHECK(slots[1].is_possible(PartitionType::PRIMARY));
    BOOST_CHECK(!slots[1].is_possible(PartitionType::EXTENDED));
    BOOST_CHECK(!slots[1].is_possible(PartitionType::LOGICAL));

    BOOST_CHECK_EQUAL(slots[2].region.get_start(), 21 * 2048);
    BOOST_CHECK_EQUAL(slots[2].region.get_length(), 9 * 2048);
    BOOST_CHECK_EQUAL(slots[2].number, 5);
    BOOST_CHECK_EQUAL(slots[2].name, "/dev/sda5");
    BOOST_CHECK_EQUAL(slots[2].primary_slot, false);
    BOOST_CHECK_EQUAL(slots[2].primary_possible, false);
    BOOST_CHECK_EQUAL(slots[2].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[2].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[2].logical_slot, true);
    BOOST_CHECK_EQUAL(slots[2].logical_possible, true);
    BOOST_CHECK(!slots[2].is_possible(PartitionType::PRIMARY));
    BOOST_CHECK(!slots[2].is_possible(PartitionType::EXTENDED));
    BOOST_CHECK(slots[2].is_possible(PartitionType::LOGICAL));
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
    BOOST_CHECK_EQUAL(slots[0].number, 1);
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

    BOOST_CHECK_EQUAL(sda->get_range(), 256);

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    BOOST_CHECK_EQUAL(gpt->max_primary(), 128);
    BOOST_CHECK(!gpt->extended_possible());

    gpt->create_partition("/dev/sda1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/sda2", Region(20 * 2048, 30 * 2048, 512), PartitionType::PRIMARY);

    vector<PartitionSlot> slots = gpt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 2);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 11 * 2048);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 9 * 2048);
    BOOST_CHECK_EQUAL(slots[0].number, 3);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);

    // 33 sectors at the end are used for the secondary gpt header

    BOOST_CHECK_EQUAL(slots[1].region.get_start(), 50 * 2048);
    BOOST_CHECK_EQUAL(slots[1].region.get_length(), 1000000 - 50 * 2048 - 33);
    BOOST_CHECK_EQUAL(slots[1].number, 3);
    BOOST_CHECK_EQUAL(slots[1].name, "/dev/sda3");
    BOOST_CHECK_EQUAL(slots[1].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[1].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[1].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[1].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[1].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[1].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_small_grain_and_mbr_gap)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    Topology topology;
    topology.set_minimal_grain(8 * KiB);
    sda->set_topology(topology);

    Msdos* msdos = to_msdos(sda->create_partition_table(PtType::MSDOS));
    msdos->set_minimal_mbr_gap(8 * KiB);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 16);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 1000000 - 16);
    BOOST_CHECK_EQUAL(slots[0].number, 1);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/sda1");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, true);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_dasd1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    // 601020 sectors * 4 KiB = 2347.734375 MiB

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", Region(0, 601020, 4096));

    PartitionTable* dasd_pt = dasda->create_partition_table(PtType::DASD);

    // 1 MiB = 256 sectors

    dasd_pt->create_partition("/dev/dasda1", Region(3 * 256, 510 * 256, 4096), PartitionType::PRIMARY);
    dasd_pt->create_partition("/dev/dasda2", Region(1023 * 256, 510 * 256, 4096), PartitionType::PRIMARY);

    vector<PartitionSlot> slots = dasd_pt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 2);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 513 * 256);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 510 * 256);
    BOOST_CHECK_EQUAL(slots[0].number, 2);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/dasda2");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);

    BOOST_CHECK_EQUAL(slots[1].region.get_start(), 1533 * 256);
    BOOST_CHECK_EQUAL(slots[1].region.get_length(), 814.734375 * 256);
    BOOST_CHECK_EQUAL(slots[1].number, 3);
    BOOST_CHECK_EQUAL(slots[1].name, "/dev/dasda3");
    BOOST_CHECK_EQUAL(slots[1].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[1].primary_possible, true);
    BOOST_CHECK_EQUAL(slots[1].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[1].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[1].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[1].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_msdos4)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", Region(0, 1000000, 512));
    dasda->set_type(DasdType::FBA);

    BOOST_CHECK_EQUAL(dasda->get_range(), 4);

    PartitionTable* msdos = dasda->create_partition_table(PtType::MSDOS);

    BOOST_CHECK_EQUAL(msdos->max_primary(), 3);
    BOOST_CHECK(msdos->extended_possible());
    BOOST_CHECK_EQUAL(msdos->max_logical(), 0);

    msdos->create_partition("/dev/dasda1", Region(2048, 2048, 512), PartitionType::PRIMARY);
    msdos->create_partition("/dev/dasda2", Region(4096, 2048, 512), PartitionType::PRIMARY);
    msdos->create_partition("/dev/dasda3", Region(6144, 2048, 512), PartitionType::PRIMARY);

    vector<PartitionSlot> slots = msdos->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 8192);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 991808);
    BOOST_CHECK_EQUAL(slots[0].number, 4);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/dasda4");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, false);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, true);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
}


BOOST_AUTO_TEST_CASE(test_gpt2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", Region(0, 1000000, 512));
    dasda->set_type(DasdType::FBA);

    BOOST_CHECK_EQUAL(dasda->get_range(), 4);

    PartitionTable* gpt = dasda->create_partition_table(PtType::GPT);

    BOOST_CHECK_EQUAL(gpt->max_primary(), 3);

    gpt->create_partition("/dev/dasda1", Region(2048, 2048, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/dasda2", Region(4096, 2048, 512), PartitionType::PRIMARY);
    gpt->create_partition("/dev/dasda3", Region(6144, 2048, 512), PartitionType::PRIMARY);

    vector<PartitionSlot> slots = gpt->get_unused_partition_slots();

    BOOST_CHECK_EQUAL(slots.size(), 1);

    BOOST_CHECK_EQUAL(slots[0].region.get_start(), 8192);
    BOOST_CHECK_EQUAL(slots[0].region.get_length(), 991775);
    BOOST_CHECK_EQUAL(slots[0].number, 4);
    BOOST_CHECK_EQUAL(slots[0].name, "/dev/dasda4");
    BOOST_CHECK_EQUAL(slots[0].primary_slot, true);
    BOOST_CHECK_EQUAL(slots[0].primary_possible, false);
    BOOST_CHECK_EQUAL(slots[0].extended_slot, false);
    BOOST_CHECK_EQUAL(slots[0].extended_possible, false);
    BOOST_CHECK_EQUAL(slots[0].logical_slot, false);
    BOOST_CHECK_EQUAL(slots[0].logical_possible, false);
}

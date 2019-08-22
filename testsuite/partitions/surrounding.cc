
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


const unsigned long long spg = GiB / 512;	// sectors per GiB


BOOST_AUTO_TEST_CASE(test_gpt)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 160 * spg, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(10 * spg, 10 * spg, 512), PartitionType::PRIMARY);
    Partition* sda2 = gpt->create_partition("/dev/sda2", Region(40 * spg, 20 * spg, 512), PartitionType::PRIMARY);
    Partition* sda3 = gpt->create_partition("/dev/sda3", Region(80 * spg, 40 * spg, 512), PartitionType::PRIMARY);

    Region surrounding_sda1 = sda1->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda1.get_start(), 34);
    BOOST_CHECK_EQUAL(surrounding_sda1.get_end(), 40 * spg - 1);

    Region surrounding_sda2 = sda2->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda2.get_start(), 20 * spg);
    BOOST_CHECK_EQUAL(surrounding_sda2.get_end(), 80 * spg - 1);

    Region surrounding_sda3 = sda3->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda3.get_start(), 60 * spg);
    BOOST_CHECK_EQUAL(surrounding_sda3.get_end(), 160 * spg - 33 - 1);
}


BOOST_AUTO_TEST_CASE(test_msdos)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 160 * spg, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(10 * spg, 10 * spg, 512), PartitionType::PRIMARY);
    Partition* sda2 = msdos->create_partition("/dev/sda2", Region(40 * spg, 100 * spg, 512), PartitionType::EXTENDED);
    Partition* sda5 = msdos->create_partition("/dev/sda5", Region(60 * spg, 15 * spg, 512), PartitionType::LOGICAL);
    Partition* sda6 = msdos->create_partition("/dev/sda6", Region(90 * spg, 25 * spg, 512), PartitionType::LOGICAL);

    Region surrounding_sda1 = sda1->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda1.get_start(), 2048);
    BOOST_CHECK_EQUAL(surrounding_sda1.get_end(), 40 * spg - 1);

    Region surrounding_sda2 = sda2->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda2.get_start(), 20 * spg);
    BOOST_CHECK_EQUAL(surrounding_sda2.get_end(), 160 * spg - 1);

    Region surrounding_sda5 = sda5->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda5.get_start(), 40 * spg + 128);
    BOOST_CHECK_EQUAL(surrounding_sda5.get_end(), 90 * spg - 1);

    Region surrounding_sda6 = sda6->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda6.get_start(), 75 * spg + 128);
    BOOST_CHECK_EQUAL(surrounding_sda6.get_end(), 140 * spg - 1);
}


BOOST_AUTO_TEST_CASE(test_msdos_partition_mbr_gap)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 10 * spg, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    /*
     * [------------- MBR gap ---------------][----------- rest -----------]
     * [0 --------------][1024 --------------][2048 -----------------------]
     *                   [        sda1       ][   sda2   ]
     */
    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(1024, 1024, 512), PartitionType::PRIMARY);
    Partition* sda2 = msdos->create_partition("/dev/sda2", Region(2048, 1 * spg, 512), PartitionType::PRIMARY);

    Region surrounding_sda1 = sda1->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda1.get_start(), 1024);
    BOOST_CHECK_EQUAL(surrounding_sda1.get_end(), 2047);

    Region surrounding_sda2 = sda2->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda2.get_start(), 2048);
    BOOST_CHECK_EQUAL(surrounding_sda2.get_end(), 10 * spg - 1);
}


BOOST_AUTO_TEST_CASE(test_msdos_partitions_inside_mbr_gap)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 10 * spg, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    /*
     * [---------------- MBR gap ---------------][-------------- rest ---------------]
     * [0 --][100 ----][200 ----][1000 ------][2000 ---------------------------------]
     *       [  sda1  ][  free  ][    sda2   ][   sda3   ]
     */
    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(100, 100, 512), PartitionType::PRIMARY);
    Partition* sda2 = msdos->create_partition("/dev/sda2", Region(1000, 1000, 512), PartitionType::PRIMARY);
    Partition* sda3 = msdos->create_partition("/dev/sda2", Region(2000, 1 * spg, 512), PartitionType::PRIMARY);

    Region surrounding_sda1 = sda1->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda1.get_start(), 100);
    BOOST_CHECK_EQUAL(surrounding_sda1.get_end(), 999);

    Region surrounding_sda2 = sda2->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda2.get_start(), 1000);
    BOOST_CHECK_EQUAL(surrounding_sda2.get_end(), 1999);

    Region surrounding_sda3 = sda3->get_unused_surrounding_region();
    BOOST_CHECK_EQUAL(surrounding_sda3.get_start(), 2000);
    BOOST_CHECK_EQUAL(surrounding_sda3.get_end(), 10 * spg - 1);
}

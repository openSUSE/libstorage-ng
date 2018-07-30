
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/MultipathImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_create1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));
    sda->get_impl().set_udev_paths({ "pci-0000:00:1f.2-ata-1" });
    sda->get_impl().set_udev_ids({ "ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683", "scsi-350014ee203733bb5" });

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);

    vector<Partition*> partitions = msdos->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 1);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/sda1");

    BOOST_REQUIRE_EQUAL(partitions[0]->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_paths()[0], "pci-0000:00:1f.2-ata-1-part1");

    BOOST_REQUIRE_EQUAL(partitions[0]->get_udev_ids().size(), 2);
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[0], "ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part1");
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[1], "scsi-350014ee203733bb5-part1");
}


BOOST_AUTO_TEST_CASE(test_create2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Multipath* multipath = Multipath::create(devicegraph, "/dev/mapper/36005076305ffc73a00000000000013b4", Region(0, 1000000, 512));
    multipath->get_impl().set_udev_paths({ });
    multipath->get_impl().set_udev_ids({ "scsi-36005076305ffc73a00000000000013b4", "wwn-0x6005076305ffc73a00000000000013b4",
		"dm-name-36005076305ffc73a00000000000013b4", "dm-uuid-mpath-36005076305ffc73a00000000000013b4" });

    PartitionTable* gpt = multipath->create_partition_table(PtType::GPT);

    gpt->create_partition("/dev/mapper/36005076305ffc73a00000000000013b4-part1", Region(1 * 2048, 10 * 2048, 512), PartitionType::PRIMARY);

    vector<Partition*> partitions = gpt->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 1);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/mapper/36005076305ffc73a00000000000013b4-part1");

    BOOST_REQUIRE_EQUAL(partitions[0]->get_udev_paths().size(), 0);

    BOOST_REQUIRE_EQUAL(partitions[0]->get_udev_ids().size(), 4);
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[0], "scsi-36005076305ffc73a00000000000013b4-part1");
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[1], "wwn-0x6005076305ffc73a00000000000013b4-part1");
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[2], "dm-name-36005076305ffc73a00000000000013b4-part1");
    BOOST_CHECK_EQUAL(partitions[0]->get_udev_ids()[3], "dm-uuid-part1-mpath-36005076305ffc73a00000000000013b4");
}

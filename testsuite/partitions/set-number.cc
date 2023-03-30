
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));
    sda->get_impl().set_sysfs_name("sda");
    sda->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda");
    sda->get_impl().set_udev_paths({ "pci-0000:00:1f.2-ata-1" });
    sda->get_impl().set_udev_ids({ "ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683", "scsi-350014ee203733bb5" });

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    msdos->create_partition("/dev/sda1", Region(1 * 2048, 100 * 2048, 512), PartitionType::EXTENDED);

    Partition* sda5 = msdos->create_partition("/dev/sda5", Region(2 * 2048, 10 * 2048, 512), PartitionType::LOGICAL);

    sda5->get_impl().set_number(6);

    vector<Partition*> partitions = msdos->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 2);

    BOOST_CHECK_EQUAL(partitions[1]->get_name(), "/dev/sda6");
    BOOST_CHECK_EQUAL(partitions[1]->get_number(), 6);

    BOOST_CHECK_EQUAL(partitions[1]->get_sysfs_name(), "sda6");
    BOOST_CHECK_EQUAL(partitions[1]->get_sysfs_path(), "/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda6");

    BOOST_REQUIRE_EQUAL(partitions[1]->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_paths()[0], "pci-0000:00:1f.2-ata-1-part6");

    BOOST_REQUIRE_EQUAL(partitions[1]->get_udev_ids().size(), 2);
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_ids()[0], "ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part6");
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_ids()[1], "scsi-350014ee203733bb5-part6");
}


BOOST_AUTO_TEST_CASE(test2)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* nvme0n1 = Disk::create(staging, "/dev/nvme0n1");
    nvme0n1->set_size(16 * GiB);
    nvme0n1->get_impl().set_sysfs_name("nvme0n1");
    nvme0n1->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1d.0/0000:3c:00.0/nvme/nvme0/nvme0n1");
    nvme0n1->get_impl().set_udev_paths({ "pci-0000:3c:00.0-nvme-1" });
    nvme0n1->get_impl().set_udev_ids({ "nvme-eui.008738c571d83492" });

    Gpt* gpt = to_gpt(nvme0n1->create_partition_table(PtType::GPT));
    Partition* partition = gpt->create_partition("/dev/nvme0n1p1", Region(2048, 1048576, 512), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(partition->get_name(), "/dev/nvme0n1p1");
    BOOST_CHECK_EQUAL(partition->get_number(), 1);

    BOOST_CHECK_EQUAL(partition->get_sysfs_name(), "nvme0n1p1");
    BOOST_CHECK_EQUAL(partition->get_sysfs_path(), "/devices/pci0000:00/0000:00:1d.0/0000:3c:00.0/nvme/nvme0/nvme0n1/nvme0n1p1");

    BOOST_REQUIRE_EQUAL(partition->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(partition->get_udev_paths()[0], "pci-0000:3c:00.0-nvme-1-part1");

    BOOST_REQUIRE_EQUAL(partition->get_udev_ids().size(), 1);
    BOOST_CHECK_EQUAL(partition->get_udev_ids()[0], "nvme-eui.008738c571d83492-part1");

    partition->get_impl().set_number(2);

    BOOST_CHECK_EQUAL(partition->get_name(), "/dev/nvme0n1p2");
    BOOST_CHECK_EQUAL(partition->get_number(), 2);

    BOOST_CHECK_EQUAL(partition->get_sysfs_name(), "nvme0n1p2");
    BOOST_CHECK_EQUAL(partition->get_sysfs_path(), "/devices/pci0000:00/0000:00:1d.0/0000:3c:00.0/nvme/nvme0/nvme0n1/nvme0n1p2");

    BOOST_REQUIRE_EQUAL(partition->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(partition->get_udev_paths()[0], "pci-0000:3c:00.0-nvme-1-part2");

    BOOST_REQUIRE_EQUAL(partition->get_udev_ids().size(), 1);
    BOOST_CHECK_EQUAL(partition->get_udev_ids()[0], "nvme-eui.008738c571d83492-part2");
}

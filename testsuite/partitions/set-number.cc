
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


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

    BOOST_CHECK_EQUAL(partitions[1]->get_sysfs_name(), "sda6");

    BOOST_CHECK_EQUAL(partitions[1]->get_sysfs_path(), "/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda6");

    BOOST_REQUIRE_EQUAL(partitions[1]->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_paths()[0], "pci-0000:00:1f.2-ata-1-part6");

    BOOST_REQUIRE_EQUAL(partitions[1]->get_udev_ids().size(), 2);
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_ids()[0], "ata-WDC_WD10EADS-00M2B0_WD-WCAV52321683-part6");
    BOOST_CHECK_EQUAL(partitions[1]->get_udev_ids()[1], "scsi-350014ee203733bb5-part6");
}

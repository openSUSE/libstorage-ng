
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Holder.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);
    sda->get_impl().set_sysfs_name("sda");
    sda->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda");
    sda->get_impl().set_udev_paths({ "pci-0000:00:1f.2-ata-1.0" });
    sda->get_impl().set_udev_ids({ "scsi-SATA_VBOX_HARDDISK_VB098dbc19-95da593f" });

    Disk* sdb = Disk::create(staging, "/dev/sdb");
    sdb->set_size(16 * GiB);
    sdb->get_impl().set_sysfs_name("sdb");
    sdb->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1f.2/ata2/host1/target1:0:0/1:0:0:0/block/sdb");
    sdb->get_impl().set_udev_paths({ "pci-0000:00:1f.2-ata-2.0" });
    sdb->get_impl().set_udev_ids({ "scsi-SATA_VBOX_HARDDISK_VB20a8f410-8a3f17dc" });

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));
    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1048576, 512), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(sda1->get_name(), "/dev/sda1");
    BOOST_CHECK_EQUAL(sda1->get_sysfs_name(), "sda1");
    BOOST_CHECK_EQUAL(sda1->get_sysfs_path(), "/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1");
    BOOST_CHECK_EQUAL(sda1->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(sda1->get_udev_paths()[0], "pci-0000:00:1f.2-ata-1.0-part1");
    BOOST_CHECK_EQUAL(sda1->get_udev_ids().size(), 1);
    BOOST_CHECK_EQUAL(sda1->get_udev_ids()[0], "scsi-SATA_VBOX_HARDDISK_VB098dbc19-95da593f-part1");

    Holder* holder = staging->find_holder(sda->get_sid(), gpt->get_sid());
    holder->set_source(sdb);

    BOOST_CHECK_EQUAL(sda1->get_name(), "/dev/sdb1");
    BOOST_CHECK_EQUAL(sda1->get_sysfs_name(), "sdb1");
    BOOST_CHECK_EQUAL(sda1->get_sysfs_path(), "/devices/pci0000:00/0000:00:1f.2/ata2/host1/target1:0:0/1:0:0:0/block/sdb/sdb1");
    BOOST_CHECK_EQUAL(sda1->get_udev_paths().size(), 1);
    BOOST_CHECK_EQUAL(sda1->get_udev_paths()[0], "pci-0000:00:1f.2-ata-2.0-part1");
    BOOST_CHECK_EQUAL(sda1->get_udev_ids().size(), 1);
    BOOST_CHECK_EQUAL(sda1->get_udev_ids()[0], "scsi-SATA_VBOX_HARDDISK_VB20a8f410-8a3f17dc-part1");
}

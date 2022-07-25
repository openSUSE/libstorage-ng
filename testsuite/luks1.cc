
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/MountPointImpl.h"
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

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 5000000, 512));
    sda->get_impl().set_udev_ids({ "scsi-350014ee203733bb5" });

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1000000, 512), PartitionType::PRIMARY);

    Luks* luks = to_luks(sda1->create_encryption("cr-test", EncryptionType::LUKS2));

    Xfs* xfs = to_xfs(luks->create_blk_filesystem(FsType::XFS));

    MountPoint* mount_point = xfs->create_mount_point("/test");

    {
	BOOST_CHECK_EQUAL(luks->get_impl().get_mount_by_name(MountByType::DEVICE), "/dev/sda1");
	BOOST_CHECK_EQUAL(luks->get_impl().get_mount_by_name(MountByType::ID), "/dev/disk/by-id/scsi-350014ee203733bb5-part1");

	BOOST_CHECK_EQUAL(mount_point->get_impl().get_mount_by_name(), "/dev/mapper/cr-test");
    }
}

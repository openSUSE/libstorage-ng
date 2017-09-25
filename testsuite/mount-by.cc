
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/User.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Utils/HumanString.h"
#include "storage/Filesystems/Mountable.h"
#include "storage/Filesystems/MountPoint.h"


using namespace storage;


namespace std
{
    ostream&
    operator<<(ostream& s, MountByType mount_my)
    {
	return s << get_mount_by_name(mount_my);
    }
}


BOOST_AUTO_TEST_CASE(test_global)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    BOOST_CHECK_EQUAL(storage.get_default_mount_by(), MountByType::UUID);

    storage.set_default_mount_by(MountByType::ID);

    BOOST_CHECK_EQUAL(storage.get_default_mount_by(), MountByType::ID);
}


BOOST_AUTO_TEST_CASE(test_partitions)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    {
	storage.set_default_mount_by(MountByType::UUID);

	Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
	Ext4* ext4 = to_ext4(sda1->create_blk_filesystem(FsType::EXT4));

	MountPoint* mount_point = ext4->create_mount_point("/test");

	BOOST_CHECK_EQUAL(mount_point->get_mount_by(), MountByType::UUID);
    }

    {
	storage.set_default_mount_by(MountByType::PATH);

	Partition* sda2 = gpt->create_partition("/dev/sda2", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
	Ext4* ext4 = to_ext4(sda2->create_blk_filesystem(FsType::EXT4));

	MountPoint* mount_point = ext4->create_mount_point("/test");

	BOOST_CHECK_EQUAL(mount_point->get_mount_by(), MountByType::PATH);
    }

    {
	storage.set_default_mount_by(MountByType::UUID);

	Partition* sda2 = gpt->create_partition("/dev/sda2", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
	Btrfs* btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
	BtrfsSubvolume* top_level = btrfs->get_top_level_btrfs_subvolume();
	BtrfsSubvolume* subvolume = top_level->create_btrfs_subvolume("test");

	MountPoint* mount_point1 = btrfs->create_mount_point("/test1");
	MountPoint* mount_point2 = top_level->create_mount_point("/test2");
	MountPoint* mount_point3 = subvolume->create_mount_point("/test3");

	BOOST_CHECK_EQUAL(mount_point1->get_mount_by(), MountByType::UUID);
	BOOST_CHECK_EQUAL(mount_point2->get_mount_by(), MountByType::UUID);
	BOOST_CHECK_EQUAL(mount_point3->get_mount_by(), MountByType::UUID);
    }
}


BOOST_AUTO_TEST_CASE(test_lvm_lvs)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 1000000, 512));

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);

    LvmVg* lvm_vg = LvmVg::create(devicegraph, "test");
    User::create(devicegraph, sda1, lvm_vg);

    {
	storage.set_default_mount_by(MountByType::UUID);

	LvmLv* lvm_lv = lvm_vg->create_lvm_lv("lv1", LvType::NORMAL, 1 * GiB);
	Ext4* ext4 = to_ext4(lvm_lv->create_blk_filesystem(FsType::EXT4));

	MountPoint* mount_point = ext4->create_mount_point("/test");

	BOOST_CHECK_EQUAL(mount_point->get_mount_by(), MountByType::DEVICE);
    }
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream&
    operator<<(ostream& s, MountByType mount_my)
    {
	return s << get_mount_by_name(mount_my);
    }
}


BOOST_AUTO_TEST_CASE(btrfs1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 16 * GiB);

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 10000000, 512), PartitionType::PRIMARY);

    Btrfs* btrfs = to_btrfs(sda1->create_blk_filesystem(FsType::BTRFS));

    BtrfsSubvolume* top_level = btrfs->get_top_level_btrfs_subvolume();

    BtrfsSubvolume* subvolume1 = top_level->create_btrfs_subvolume("test1");
    MountPoint* mount_point1 = subvolume1->create_mount_point("/test1");

    BtrfsSubvolume* subvolume2 = top_level->create_btrfs_subvolume("test2");

    BOOST_CHECK_EQUAL(btrfs->get_btrfs_subvolumes().size(), 3);

    BOOST_CHECK_THROW(btrfs->get_mount_point(), Exception);
    BOOST_CHECK_THROW(top_level->get_mount_point(), Exception);
    BOOST_CHECK_EQUAL(subvolume1->get_mount_point(), mount_point1);
    BOOST_CHECK_THROW(subvolume2->get_mount_point(), Exception);

    BOOST_CHECK_EQUAL(btrfs->get_filesystem(), btrfs);
    BOOST_CHECK_EQUAL(top_level->get_filesystem(), btrfs);
    BOOST_CHECK_EQUAL(subvolume1->get_filesystem(), btrfs);
    BOOST_CHECK_EQUAL(subvolume2->get_filesystem(), btrfs);

    BOOST_CHECK_EQUAL(mount_point1->get_mount_by(), MountByType::UUID);

    // The mount point for a subvolume uses the btrfs filesystem to write
    // entries in /etc/fstab. As long as the UUID in unknown the kernel name
    // is used.

    BOOST_CHECK_EQUAL(mount_point1->get_impl().get_mount_by_name(), "/dev/sda1");

    btrfs->set_uuid("dead-beef");
    BOOST_CHECK_EQUAL(mount_point1->get_impl().get_mount_by_name(), "UUID=dead-beef");
}

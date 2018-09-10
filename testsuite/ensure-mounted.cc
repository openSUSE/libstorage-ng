
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Nfs.h"
#include "storage/Filesystems/MountableImpl.h"
#include "storage/Environment.h"
#include "storage/Storage.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 2097152, 512), PartitionType::PRIMARY);
    BlkFilesystem* blk_filesystem1 = sda1->create_blk_filesystem(FsType::EXT4);
    blk_filesystem1->create_mount_point("/test1");

    Nfs* nfs1 = Nfs::create(staging, "192.168.1.1", "/export");
    nfs1->create_mount_point("/test3");

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    Partition* sda2 = gpt->create_partition("/dev/sda2", Region(2099200, 2097152, 512), PartitionType::PRIMARY);
    BlkFilesystem* blk_filesystem2 = sda2->create_blk_filesystem(FsType::EXT4);

    {
	// blk_filesystem1 is mounted at /test1 in system.

	EnsureMounted ensure_mounted(blk_filesystem1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test1");
    }

    blk_filesystem1->remove_mount_point();

    {
	// blk_filesystem1 is mounted at /test1 in system. The mount
	// point in staging does not matter.

	EnsureMounted ensure_mounted(blk_filesystem1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test1");
    }

    blk_filesystem1->create_mount_point("/test2");

    {
	// blk_filesystem1 is mounted at /test1 in system. The mount
	// point in staging does not matter.

	EnsureMounted ensure_mounted(blk_filesystem1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test1");
    }

    {
	// blk_filesystem2 does not exist in system.

	BOOST_CHECK_THROW({ EnsureMounted ensure_mounted(blk_filesystem2, false); }, Exception);
    }

    {
	// nfs1 is mounted at /test3 in system. The mount point in
	// staging does not matter.

	EnsureMounted ensure_mounted(nfs1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test3");
    }

    nfs1->remove_mount_point();

    {
	// nfs1 is mounted at /test1 in system. The mount point in
	// staging does not matter.

	EnsureMounted ensure_mounted(nfs1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test3");
    }

    nfs1->create_mount_point("/test4");

    {
	// nfs1 is mounted at /test1 in system. The mount point in
	// staging does not matter.

	EnsureMounted ensure_mounted(nfs1);

	BOOST_CHECK_EQUAL(ensure_mounted.get_any_mount_point(), "/test3");
    }
}

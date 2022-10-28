
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Xfs.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"

#include "testsuite/helpers/TsCmp.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(ext4)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(1 * TiB);

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1000000, 512), PartitionType::PRIMARY);

    Ext4* ext4 = to_ext4(sda1->create_blk_filesystem(FsType::EXT4));

    MountPoint* mount_point = ext4->create_mount_point("/test");

    {
	mount_point->set_mount_options({ });

	BOOST_CHECK_EQUAL(required_features(staging), "ext4");
	BOOST_CHECK_EQUAL(suggested_features(staging), "ext4");
    }

    {
	mount_point->set_mount_options({ "quota", "data=ordered" });

	BOOST_CHECK_EQUAL(required_features(staging), "ext4 quota");
	BOOST_CHECK_EQUAL(suggested_features(staging), "ext4 quota");
    }

    {
	mount_point->set_mount_options({ "noquota" });

	BOOST_CHECK_EQUAL(required_features(staging), "ext4");
	BOOST_CHECK_EQUAL(suggested_features(staging), "ext4");
    }

    {
	mount_point->set_mount_options({ "prjquota" });

	BOOST_CHECK_EQUAL(required_features(staging), "ext4 quota");
	BOOST_CHECK_EQUAL(suggested_features(staging), "ext4 quota");
    }

    {
	mount_point->set_mount_options({ "usrjquota=aquota.user" });

	BOOST_CHECK_EQUAL(required_features(staging), "ext4 quota");
	BOOST_CHECK_EQUAL(suggested_features(staging), "ext4 quota");
    }
}


BOOST_AUTO_TEST_CASE(xfs)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(1 * TiB);

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1000000, 512), PartitionType::PRIMARY);

    Xfs* xfs = to_xfs(sda1->create_blk_filesystem(FsType::XFS));

    MountPoint* mount_point = xfs->create_mount_point("/test");

    {
	mount_point->set_mount_options({ });

	BOOST_CHECK_EQUAL(required_features(staging), "xfs");
	BOOST_CHECK_EQUAL(suggested_features(staging), "xfs");
    }

    {
	mount_point->set_mount_options({ "quota" });

	BOOST_CHECK_EQUAL(required_features(staging), "quota xfs");
	BOOST_CHECK_EQUAL(suggested_features(staging), "quota xfs");
    }

    {
	mount_point->set_mount_options({ "noquota" });

	BOOST_CHECK_EQUAL(required_features(staging), "xfs");
	BOOST_CHECK_EQUAL(suggested_features(staging), "xfs");
    }

    {
	mount_point->set_mount_options({ "pquota" });

	BOOST_CHECK_EQUAL(required_features(staging), "quota xfs");
	BOOST_CHECK_EQUAL(suggested_features(staging), "quota xfs");
    }
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/LvmVgImpl.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"
#include "storage/FreeInfo.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 32 * GiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    LvmLv* thin_pool = lvm_vg->create_lvm_lv("thin-pool", LvType::THIN_POOL, 1 * GiB);

    LvmLv* thin = thin_pool->create_lvm_lv("thin", LvType::THIN, 64 * GiB);

    {
	ResizeInfo resize_info = thin_pool->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 4 * MiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, (32 * GiB - 4 * MiB) - 2 * 32 * MiB);
	BOOST_CHECK_EQUAL(resize_info.block_size, 4 * MiB);
    }

    {
	ResizeInfo resize_info = thin->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 4 * MiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 16 * PiB - 4 * MiB);
	BOOST_CHECK_EQUAL(resize_info.block_size, 4 * MiB);
    }
}


BOOST_AUTO_TEST_CASE(test2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 128 * MiB);
    Disk* sdb = Disk::create(staging, "/dev/sdb", 128 * MiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);
    lvm_vg->add_lvm_pv(sdb);

    LvmLv* normal = lvm_vg->create_lvm_lv("normal", LvType::NORMAL, 40 * MiB);
    normal->set_stripes(2);

    BlkFilesystem* fs1 = normal->create_blk_filesystem(FsType::EXT4);
    fs1->get_impl().set_resize_info(ResizeInfo(true, 0, 33 * MiB, 4 * TiB));

    LvmLv* thin_pool = lvm_vg->create_lvm_lv("thin-pool", LvType::THIN_POOL, 60 * MiB);

    LvmLv* thin = thin_pool->create_lvm_lv("thin", LvType::THIN, 1 * GiB);

    BlkFilesystem* fs2 = thin->create_blk_filesystem(FsType::EXT4);
    fs2->get_impl().set_resize_info(ResizeInfo(true, 0, 33 * MiB, 4 * TiB));

    lvm_vg->get_impl().set_reserved_extents(2);

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    {
	ResizeInfo resize_info = normal->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 40 * MiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, (2 * (128 - 4) - 2 * 4 - 60 - 4) * MiB);
	BOOST_CHECK_EQUAL(resize_info.block_size, 8 * MiB);
    }

    {
	ResizeInfo resize_info = thin_pool->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 60 * MiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, (2 * (128 - 4) - 2 * 4 - 40) * MiB);
	BOOST_CHECK_EQUAL(resize_info.block_size, 4 * MiB);
    }

    {
	ResizeInfo resize_info = thin->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 36 * MiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 4 * TiB);
	BOOST_CHECK_EQUAL(resize_info.block_size, 4 * MiB);
    }
}

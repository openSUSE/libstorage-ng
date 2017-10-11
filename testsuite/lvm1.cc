
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLvImpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(sizes1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 2 * TiB);
    Disk* sdb = Disk::create(staging, "/dev/sdb", 2 * TiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);
    lvm_vg->add_lvm_pv(sdb);

    // one extent for metadata per PV
    BOOST_CHECK_EQUAL(lvm_vg->number_of_extents(), 4 * TiB / (4 * MiB) - 2);

    BOOST_CHECK_EQUAL(lvm_vg->number_of_used_extents(), 0);
    BOOST_CHECK_EQUAL(lvm_vg->number_of_free_extents(), 4 * TiB / (4 * MiB) - 2);

    BOOST_CHECK_EQUAL(lvm_vg->max_size_for_lvm_lv(LvType::NORMAL), lvm_vg->number_of_free_extents() * 4 * MiB);

    // twice 128 MiB for metadata
    BOOST_CHECK_EQUAL(lvm_vg->max_size_for_lvm_lv(LvType::THIN_POOL), lvm_vg->number_of_free_extents() * 4 * MiB - 256 * MiB);

    unsigned long long used_extents = 0;

    {
	LvmLv* thin_pool1 = lvm_vg->create_lvm_lv("thin-pool1", LvType::THIN_POOL, 1 * GiB);

	BOOST_CHECK_EQUAL(thin_pool1->get_chunk_size(), 0 * B);

	BOOST_CHECK_EQUAL(thin_pool1->get_impl().default_chunk_size(), 64 * KiB);
	BOOST_CHECK_EQUAL(thin_pool1->get_impl().default_metadata_size(), 4 * MiB);

	BOOST_CHECK_EQUAL(thin_pool1->max_size_for_lvm_lv(LvType::THIN), 16 * PiB - 4 * MiB);

	used_extents += (1 * GiB) / (4 * MiB); // data
	used_extents += (4 * MiB) / (4 * MiB); // metadata
    }

    {
	LvmLv* thin_pool2 = lvm_vg->create_lvm_lv("thin-pool2", LvType::THIN_POOL, 1 * TiB);

	BOOST_CHECK_EQUAL(thin_pool2->get_chunk_size(), 0 * B);

	BOOST_CHECK_EQUAL(thin_pool2->get_impl().default_chunk_size(), 512 * KiB);
	BOOST_CHECK_EQUAL(thin_pool2->get_impl().default_metadata_size(), 128 * MiB);

	used_extents += (1 * TiB) / (4 * MiB); // data
	used_extents += (128 * MiB) / (4 * MiB); // metadata
	used_extents += (128 * MiB) / (4 * MiB); // spare metadata
    }

    {
	LvmLv* thin_pool3 = lvm_vg->create_lvm_lv("thin-pool3", LvType::THIN_POOL, 1 * TiB);
	thin_pool3->set_chunk_size(4 * MiB);

	BOOST_CHECK_EQUAL(thin_pool3->get_chunk_size(), 4 * MiB);

	BOOST_CHECK_EQUAL(thin_pool3->get_impl().default_chunk_size(), 512 * KiB);
	BOOST_CHECK_EQUAL(thin_pool3->get_impl().default_metadata_size(), 16 * MiB);

	used_extents += (1 * TiB) / (4 * MiB); // data
	used_extents += (16 * MiB) / (4 * MiB); // metadata
    }

    BOOST_CHECK_EQUAL(lvm_vg->number_of_used_extents(), used_extents);
    BOOST_CHECK_EQUAL(lvm_vg->number_of_free_extents(), 4 * TiB / (4 * MiB) - 2 - used_extents);
}


BOOST_AUTO_TEST_CASE(lvm_vg_overcommitted)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 1 * TiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    lvm_vg->create_lvm_lv("normal1", LvType::NORMAL, 600 * GiB);

    BOOST_CHECK(!lvm_vg->is_overcommitted());

    lvm_vg->create_lvm_lv("normal2", LvType::NORMAL, 600 * GiB);

    BOOST_CHECK(lvm_vg->is_overcommitted());
}


BOOST_AUTO_TEST_CASE(chunk_size_too_small)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 20 * TiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    LvmLv* thin_pool = lvm_vg->create_lvm_lv("thin-pool", LvType::THIN_POOL, 16 * TiB);
    thin_pool->set_chunk_size(64 * KiB);

    // TODO check correct reason ("chunk size too small"), needs better API
    // for check function
    BOOST_CHECK_THROW(staging->check(), Exception);
}

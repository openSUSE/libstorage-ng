
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Holders/User.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


unsigned long long extent_size = 4 * MiB;


BOOST_AUTO_TEST_CASE(add_lvm_pv_test1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    BOOST_CHECK_EQUAL(staging->num_devices(), 3);
    BOOST_CHECK_EQUAL(staging->num_holders(), 2);

    BOOST_CHECK_EQUAL(sda->num_children(), 1);
    BOOST_CHECK_EQUAL(lvm_vg->get_lvm_pvs().size(), 1);

    BOOST_CHECK_EQUAL(lvm_vg->get_size(), 16 * GiB - extent_size);
    BOOST_CHECK_EQUAL(lvm_vg->get_extent_size(), extent_size);
}


BOOST_AUTO_TEST_CASE(add_lvm_pv_test2)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);

    LvmPv* lvm_pv = LvmPv::create(staging);
    User::create(staging, sda, lvm_pv);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    BOOST_CHECK_EQUAL(staging->num_devices(), 3);
    BOOST_CHECK_EQUAL(staging->num_holders(), 2);

    BOOST_CHECK_EQUAL(sda->num_children(), 1);
    BOOST_CHECK_EQUAL(lvm_vg->get_lvm_pvs().size(), 1);

    BOOST_CHECK_EQUAL(lvm_vg->get_size(), 16 * GiB - extent_size);
    BOOST_CHECK_EQUAL(lvm_vg->get_extent_size(), extent_size);
}


BOOST_AUTO_TEST_CASE(add_lvm_pv_test3)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);

    sda->create_partition_table(PtType::GPT);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    BOOST_CHECK_EXCEPTION(lvm_vg->add_lvm_pv(sda), DeviceHasWrongType,
	[](const Exception& e) { return e.what() == "device has wrong type, seen 'Gpt', expected 'LvmPv'"s; }
    );
}


BOOST_AUTO_TEST_CASE(remove_lvm_pv_test1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    lvm_vg->add_lvm_pv(sda);

    BOOST_CHECK_EQUAL(staging->num_devices(), 3);
    BOOST_CHECK_EQUAL(staging->num_holders(), 2);

    BOOST_CHECK_EQUAL(lvm_vg->get_size(), 16 * GiB - extent_size);
    BOOST_CHECK_EQUAL(lvm_vg->get_extent_size(), extent_size);

    lvm_vg->remove_lvm_pv(sda);

    BOOST_CHECK_EQUAL(staging->num_devices(), 2);
    BOOST_CHECK_EQUAL(staging->num_holders(), 0);

    BOOST_CHECK_EQUAL(lvm_vg->get_size(), 0);
    BOOST_CHECK_EQUAL(lvm_vg->get_extent_size(), extent_size);
}


BOOST_AUTO_TEST_CASE(create_lvm_lv_test1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(16 * GiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "vg-name");
    lvm_vg->add_lvm_pv(sda);

    LvmLv* lvm_lv = lvm_vg->create_lvm_lv("lv-name", LvType::NORMAL, 10 * GiB);

    BOOST_CHECK_EQUAL(staging->num_devices(), 4);
    BOOST_CHECK_EQUAL(staging->num_holders(), 3);

    BOOST_CHECK_EQUAL(lvm_lv->get_name(), "/dev/vg-name/lv-name");
    BOOST_CHECK_EQUAL(lvm_lv->get_dm_table_name(), "vg--name-lv--name");

    BOOST_CHECK_EQUAL(lvm_lv->get_size(), 10 * GiB);
    BOOST_CHECK_EQUAL(lvm_lv->get_region(), Region(0, 10 * GiB / extent_size, extent_size));
}

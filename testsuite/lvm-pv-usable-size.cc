
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(disk1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 1 * GiB);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    const LvmPv* lvm_pv = lvm_vg->add_lvm_pv(sda);

    BOOST_CHECK_EQUAL(lvm_pv->get_usable_size(), sda->get_size() - 1 * MiB);
}


BOOST_AUTO_TEST_CASE(partition1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 1 * GiB);
    Msdos* msdos = to_msdos(sda->create_partition_table(PtType::MSDOS));
    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(2048, 2095104, 512), PartitionType::PRIMARY);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    const LvmPv* lvm_pv = lvm_vg->add_lvm_pv(sda1);

    BOOST_CHECK_EQUAL(lvm_pv->get_usable_size(), sda1->get_size() - 1 * MiB);
}


BOOST_AUTO_TEST_CASE(disk2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 1 * GiB);
    sda->set_topology(Topology(0, 2 * MiB));

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    const LvmPv* lvm_pv = lvm_vg->add_lvm_pv(sda);

    BOOST_CHECK_EQUAL(lvm_pv->get_usable_size(), sda->get_size() - 2 * MiB);
}


BOOST_AUTO_TEST_CASE(partition2)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", 1 * GiB);
    sda->set_topology(Topology(0, 2 * MiB));
    Msdos* msdos = to_msdos(sda->create_partition_table(PtType::MSDOS));
    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(2048, 2095104, 512), PartitionType::PRIMARY);

    LvmVg* lvm_vg = LvmVg::create(staging, "test");
    const LvmPv* lvm_pv = lvm_vg->add_lvm_pv(sda1);

    BOOST_CHECK_EQUAL(lvm_pv->get_usable_size(), sda1->get_size() - 2 * MiB);
}

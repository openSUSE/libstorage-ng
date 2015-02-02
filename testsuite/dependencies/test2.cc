
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    // TODO
    Actiongraph::simple_t expected = {
	{ "Create gpt on /dev/sda", { "Create partition /dev/sda1 (16.00 GiB)" } },
	{ "Create partition /dev/sda1 (16.00 GiB)", { "Set id of partition /dev/sda1 to 0x8E" } },
	{ "Set id of partition /dev/sda1 to 0x8E", { "Create volume group /dev/system" } },
	{ "Create volume group /dev/system", { "Create logical volume /dev/system/root (14.00 GiB)", "Create logical volume /dev/system/swap (2.00 GiB)" } },
	{ "Create logical volume /dev/system/root (14.00 GiB)", { } },
	{ "Create logical volume /dev/system/swap (2.00 GiB)", { } }
    };

    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* lhs = storage.get_current();

    Disk::create(lhs, "/dev/sda");

    Devicegraph* rhs = storage.copy_devicegraph("current", "old");

    Disk* sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = Partition::create(rhs, "/dev/sda1", PRIMARY);
    Subdevice::create(rhs, gpt, sda1);
    sda1->set_size_k(16 * 1024 * 1024);
    sda1->set_id(ID_LVM);

    LvmVg* system = LvmVg::create(rhs, "/dev/system");
    Using::create(rhs, sda1, system);

    LvmLv* system_root = LvmLv::create(rhs, "/dev/system/root");
    Subdevice::create(rhs, system, system_root);
    system_root->set_size_k(14 * 1024 * 1024);

    LvmLv* system_swap = LvmLv::create(rhs, "/dev/system/swap");
    Subdevice::create(rhs, system, system_swap);
    system_swap->set_size_k(2 * 1024 * 1024);

    Actiongraph actiongraph(storage, lhs, rhs);

    BOOST_CHECK_EQUAL(actiongraph.get_simple(), expected);
}

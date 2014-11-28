
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"
#include "storage/Storage.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    ActionGraph::simple_t expected = {
	{ "43 create /dev/sda1", { "43 set type /dev/sda1" } },
	{ "43 set type /dev/sda1", { "44 create /dev/system" } },
	{ "44 create /dev/system", { "45 create /dev/system/root", "46 create /dev/system/swap" } },
	{ "45 create /dev/system/root", { } },
	{ "46 create /dev/system/swap", { } }
    };

    Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    DeviceGraph& lhs = *storage.getCurrent();

    Disk::create(lhs, "/dev/sda");

    DeviceGraph& rhs = *storage.copyDeviceGraph("current", "old");

    Disk* sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Partition* sda1 = Partition::create(rhs, "/dev/sda1");
    Subdevice::create(rhs, sda, sda1);

    LvmVg* system = LvmVg::create(rhs, "/dev/system");
    Using::create(rhs, sda1, system);

    LvmLv* system_root = LvmLv::create(rhs, "/dev/system/root");
    Subdevice::create(rhs, system, system_root);

    LvmLv* system_swap = LvmLv::create(rhs, "/dev/system/swap");
    Subdevice::create(rhs, system, system_swap);

    ActionGraph action_graph(lhs, rhs);

    BOOST_CHECK_EQUAL(action_graph.simple(), expected);
}

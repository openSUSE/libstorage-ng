
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/DeviceGraph.h"
#include "storage/ActionGraph.h"


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

    DeviceGraph lhs;

    new Disk(lhs, "/dev/sda");

    DeviceGraph rhs;
    lhs.copy(rhs);

    Disk* sda = dynamic_cast<Disk*>(BlkDevice::find(rhs, "/dev/sda"));

    Partition* sda1 = new Partition(rhs, "/dev/sda1");
    new Subdevice(rhs, sda, sda1);

    LvmVg* system = new LvmVg(rhs, "/dev/system");
    new Using(rhs, sda1, system);

    LvmLv* system_root = new LvmLv(rhs, "/dev/system/root");
    new Subdevice(rhs, system, system_root);

    LvmLv* system_swap = new LvmLv(rhs, "/dev/system/swap");
    new Subdevice(rhs, system, system_swap);

    ActionGraph action_graph(lhs, rhs);

    BOOST_CHECK_EQUAL(action_graph.simple(), expected);
}

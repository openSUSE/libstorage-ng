
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dependencies

#include <boost/test/unit_test.hpp>

#include "storage/Device.h"
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

    lhs.add_vertex(new Disk("/dev/sda"));

    DeviceGraph rhs;
    lhs.copy(rhs);

    DeviceGraph::vertex_descriptor sda = rhs.find_vertex("/dev/sda");

    DeviceGraph::vertex_descriptor sda1 = rhs.add_vertex(new Partition("/dev/sda1"));
    rhs.add_edge(sda, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor system = rhs.add_vertex(new LvmVg("/dev/system"));
    rhs.add_edge(sda1, system, new Using());

    DeviceGraph::vertex_descriptor system_root = rhs.add_vertex(new LvmLv("/dev/system/root"));
    rhs.add_edge(system, system_root, new Subdevice());

    DeviceGraph::vertex_descriptor system_swap = rhs.add_vertex(new LvmLv("/dev/system/swap"));
    rhs.add_edge(system, system_swap, new Subdevice());

    ActionGraph action_graph(lhs, rhs);

    BOOST_CHECK_EQUAL(action_graph.simple(), expected);
}

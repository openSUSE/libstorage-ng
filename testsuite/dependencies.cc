
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
	{ "create 43 /dev/sda1", { "set type 43 /dev/sda1" } },
	{ "set type 43 /dev/sda1", { "create 44 fake /dev/system" } },
	{ "create 44 fake /dev/system", { "create 45 /dev/system/root", "create 46 /dev/system/swap" } },
	{ "create 45 /dev/system/root", { } },
	{ "create 46 /dev/system/swap", { } }
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


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE dynamic

#include <boost/test/unit_test.hpp>

#include "storage/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dynamic)
{
    DeviceGraph device_graph;

    DeviceGraph::vertex_descriptor sda = device_graph.add_vertex(new Disk("/dev/sda"));

    DeviceGraph::vertex_descriptor sda1 = device_graph.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::edge_descriptor sda_sda1 = device_graph.add_edge(sda, sda1, new Subdevice());

    DeviceGraph::vertex_descriptor system = device_graph.add_vertex(new LvmVg("/dev/system"));
    DeviceGraph::edge_descriptor sda1_system = device_graph.add_edge(sda1, system, new Using());

    DeviceGraph::vertex_descriptor root = device_graph.add_vertex(new LvmLv("/dev/system/root"));
    DeviceGraph::edge_descriptor system_root = device_graph.add_edge(system, root, new Subdevice());

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 4);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 3);

    BOOST_CHECK(dynamic_cast<const Disk*>(device_graph.graph[sda].get()));

    BOOST_CHECK(dynamic_cast<const Partition*>(device_graph.graph[sda1].get()));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(device_graph.graph[sda_sda1].get()));

    BOOST_CHECK(dynamic_cast<const LvmVg*>(device_graph.graph[system].get()));
    BOOST_CHECK(dynamic_cast<const Using*>(device_graph.graph[sda1_system].get()));

    BOOST_CHECK(dynamic_cast<const LvmLv*>(device_graph.graph[root].get()));
    BOOST_CHECK(dynamic_cast<const Subdevice*>(device_graph.graph[system_root].get()));
}

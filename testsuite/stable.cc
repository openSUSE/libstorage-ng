
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE valid

#include <boost/test/unit_test.hpp>

#include "storage/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(valid)
{
    DeviceGraph device_graph;

    DeviceGraph::vertex_descriptor sda = device_graph.add_vertex(new Disk("/dev/sda"));
    DeviceGraph::vertex_descriptor sda1 = device_graph.add_vertex(new Partition("/dev/sda1"));
    DeviceGraph::vertex_descriptor sda2 = device_graph.add_vertex(new Partition("/dev/sda2"));

    device_graph.add_edge(sda, sda1, new Subdevice());
    device_graph.add_edge(sda, sda2, new Subdevice());

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 3);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 2);

    device_graph.remove_vertex(sda1);

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 2);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 1);

    // sda and sda2 still valid and correct due to using VertexList=boost::listS

    BOOST_CHECK_EQUAL(device_graph.graph[sda]->display_name(), "/dev/sda");
    BOOST_CHECK_EQUAL(device_graph.graph[sda2]->display_name(), "/dev/sda2");

    BOOST_CHECK_THROW(device_graph.find_vertex("/dev/sda1"), runtime_error);
}

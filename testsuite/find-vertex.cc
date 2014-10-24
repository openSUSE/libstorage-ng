
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Device.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    DeviceGraph device_graph;

    DeviceGraph::vertex_descriptor sda = device_graph.add_vertex(new Disk("/dev/sda"));

    DeviceGraph::vertex_descriptor sda1 = device_graph.add_vertex(new Partition("/dev/sda1"));
    device_graph.add_edge(sda, sda1, new Subdevice());

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 2);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 1);

    BOOST_CHECK_EQUAL(device_graph.find_vertex("/dev/sda"), sda);
    BOOST_CHECK_EQUAL(device_graph.find_vertex("/dev/sda1"), sda1);
    BOOST_CHECK_THROW(device_graph.find_vertex("/dev/sda2"), runtime_error);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/DeviceGraphImpl.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(valid)
{
    DeviceGraph device_graph;

    Disk* sda = new Disk(device_graph, "/dev/sda");

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, sda, sda1);

    Partition* sda2 = new Partition(device_graph, "/dev/sda2");
    new Subdevice(device_graph, sda, sda2);

    DeviceGraph::Impl::vertex_descriptor v_sda = sda->getImpl().getVertex();
    DeviceGraph::Impl::vertex_descriptor v_sda2 = sda2->getImpl().getVertex();

    BOOST_CHECK_EQUAL(device_graph.num_vertices(), 3);
    BOOST_CHECK_EQUAL(device_graph.num_edges(), 2);

    device_graph.check();

    device_graph.remove_vertex(sda1);

    BOOST_CHECK_EQUAL(device_graph.num_vertices(), 2);
    BOOST_CHECK_EQUAL(device_graph.num_edges(), 1);

    device_graph.check();

    // The vertex_descriptors for sda and sda2 are still valid and correct due
    // to using VertexList=boost::listS in DeviceGraph.

    BOOST_CHECK_EQUAL(v_sda, sda->getImpl().getVertex());
    BOOST_CHECK_EQUAL(v_sda2, sda2->getImpl().getVertex());

    BOOST_CHECK_EQUAL(sda->display_name(), "/dev/sda");
    BOOST_CHECK_EQUAL(sda2->display_name(), "/dev/sda2");

    BOOST_CHECK_THROW(BlkDevice::find(device_graph, "/dev/sda1"), runtime_error);
}

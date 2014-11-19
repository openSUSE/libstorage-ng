
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    DeviceGraph device_graph;

    Disk* sda = new Disk(device_graph, "/dev/sda");

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, sda, sda1);

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 2);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 1);

    BOOST_CHECK_EQUAL(device_graph.find_blk_device("/dev/sda"), sda);
    BOOST_CHECK_EQUAL(device_graph.find_blk_device("/dev/sda1"), sda1);
    BOOST_CHECK_THROW(device_graph.find_blk_device("/dev/sda2"), runtime_error);
}

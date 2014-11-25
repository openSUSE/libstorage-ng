
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

    BOOST_CHECK_EQUAL(device_graph.num_vertices(), 2);
    BOOST_CHECK_EQUAL(device_graph.num_edges(), 1);

    BOOST_CHECK_EQUAL(BlkDevice::find(device_graph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find(device_graph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find(device_graph, "/dev/sda2"), runtime_error);
}

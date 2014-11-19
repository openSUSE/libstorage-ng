
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    DeviceGraph device_graph;

    new Disk(device_graph, "/dev/sda");
    new Gpt(device_graph);
    new Partition(device_graph, "/dev/sda1");

    new Encryption(device_graph, "/dev/mapper/cr_sda1");

    new LvmVg(device_graph, "/dev/system");
    new LvmLv(device_graph, "/dev/system/root");

    new Ext4(device_graph);
    new Swap(device_graph);

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 8);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 0);

    device_graph.check();

    DeviceGraph device_graph_copy;
    device_graph.copy(device_graph_copy);

    BOOST_CHECK_EQUAL(num_vertices(device_graph_copy.graph), 8);
    BOOST_CHECK_EQUAL(num_edges(device_graph_copy.graph), 0);

    device_graph_copy.check();
}

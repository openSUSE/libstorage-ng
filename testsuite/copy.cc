
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
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    DeviceGraph* device_graph = new DeviceGraph();

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Gpt* gpt = Gpt::create(device_graph);
    Using::create(device_graph, sda, gpt);

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, gpt, sda1);

    Encryption::create(device_graph, "/dev/mapper/cr_sda1");

    LvmVg::create(device_graph, "/dev/system");
    LvmLv::create(device_graph, "/dev/system/root");

    Ext4::create(device_graph);
    Swap::create(device_graph);

    BOOST_CHECK_EQUAL(device_graph->numVertices(), 8);
    BOOST_CHECK_EQUAL(device_graph->numEdges(), 2);

    device_graph->check();

    DeviceGraph* device_graph_copy = new DeviceGraph();
    device_graph->copy(*device_graph_copy);

    BOOST_CHECK_EQUAL(device_graph_copy->numVertices(), 8);
    BOOST_CHECK_EQUAL(device_graph_copy->numEdges(), 2);

    device_graph_copy->check();

    delete device_graph;
    delete device_graph_copy;
}

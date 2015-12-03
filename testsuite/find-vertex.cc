
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", Region(0, 10, 262144), PRIMARY);
    Subdevice::create(devicegraph, sda, sda1);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 2);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 1);

    BOOST_CHECK_EQUAL(BlkDevice::find(devicegraph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find(devicegraph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find(devicegraph, "/dev/sda2"), DeviceNotFound);

    delete devicegraph;
}

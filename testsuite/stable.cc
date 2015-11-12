
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DevicegraphImpl.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(valid)
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", PRIMARY);
    Subdevice::create(devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(devicegraph, "/dev/sda2", PRIMARY);
    Subdevice::create(devicegraph, sda, sda2);

    Devicegraph::Impl::vertex_descriptor v_sda = sda->get_impl().get_vertex();
    Devicegraph::Impl::vertex_descriptor v_sda2 = sda2->get_impl().get_vertex();

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 3);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    devicegraph->check();

    devicegraph->remove_device(sda1);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 2);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 1);

    devicegraph->check();

    // The vertex_descriptors for sda and sda2 are still valid and correct due
    // to using VertexList=boost::listS in Devicegraph.

    BOOST_CHECK_EQUAL(v_sda, sda->get_impl().get_vertex());
    BOOST_CHECK_EQUAL(v_sda2, sda2->get_impl().get_vertex());

    BOOST_CHECK_EQUAL(sda->get_name(), "/dev/sda");
    BOOST_CHECK_EQUAL(sda2->get_name(), "/dev/sda2");

    BOOST_CHECK_THROW(BlkDevice::find(devicegraph, "/dev/sda1"), DeviceNotFound);

    delete devicegraph;
}

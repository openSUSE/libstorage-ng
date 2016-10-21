
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/User.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(valid)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Msdos* msdos = Msdos::create(devicegraph);
    User::create(devicegraph, sda, msdos);

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", Region(0, 10, 262144), PartitionType::PRIMARY);
    Subdevice::create(devicegraph, msdos, sda1);

    Partition* sda2 = Partition::create(devicegraph, "/dev/sda2", Region(10, 10, 262144), PartitionType::PRIMARY);
    Subdevice::create(devicegraph, msdos, sda2);

    Devicegraph::Impl::vertex_descriptor v_sda = sda->get_impl().get_vertex();
    Devicegraph::Impl::vertex_descriptor v_sda2 = sda2->get_impl().get_vertex();

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 4);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 3);

    devicegraph->check();

    devicegraph->remove_device(sda1);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 3);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    devicegraph->check();

    // The vertex_descriptors for sda and sda2 are still valid and correct due
    // to using VertexList=boost::listS in Devicegraph.

    BOOST_CHECK_EQUAL(v_sda, sda->get_impl().get_vertex());
    BOOST_CHECK_EQUAL(v_sda2, sda2->get_impl().get_vertex());

    BOOST_CHECK_EQUAL(sda->get_name(), "/dev/sda");
    BOOST_CHECK_EQUAL(sda2->get_name(), "/dev/sda2");

    BOOST_CHECK_EXCEPTION(BlkDevice::find_by_name(devicegraph, "/dev/sda1"), DeviceNotFound,
	[](const DeviceNotFound& e) { return strcmp(e.what(), "device not found, name:/dev/sda1") == 0;
    });
}

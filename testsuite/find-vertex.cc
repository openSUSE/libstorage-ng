
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", Region(0, 10, 262144), PartitionType::PRIMARY);
    Subdevice::create(devicegraph, sda, sda1);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 2);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 1);

    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find_by_name(devicegraph, "/dev/sda2"), DeviceNotFound);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1000000, 512), PartitionType::PRIMARY);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 3);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find_by_name(devicegraph, "/dev/sda2"), DeviceNotFound);

    BOOST_CHECK(sda->exists_in_devicegraph(devicegraph));
    BOOST_CHECK(!sda->exists_in_probed());
    BOOST_CHECK(sda->exists_in_staging());
}

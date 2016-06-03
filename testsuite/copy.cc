
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
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(copy)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Gpt* gpt = Gpt::create(devicegraph);
    User::create(devicegraph, sda, gpt);

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", Region(0, 10, 262144), PartitionType::PRIMARY);
    Subdevice::create(devicegraph, gpt, sda1);

    Encryption::create(devicegraph, "/dev/mapper/cr_sda1");

    LvmVg::create(devicegraph, "system");
    LvmLv::create(devicegraph, "system", "root");

    Ext4::create(devicegraph);
    Swap::create(devicegraph);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 8);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    devicegraph->check();

    Devicegraph* devicegraph_copy = storage.copy_devicegraph("staging", "copy");

    BOOST_CHECK_EQUAL(devicegraph_copy->num_devices(), 8);
    BOOST_CHECK_EQUAL(devicegraph_copy->num_holders(), 2);

    devicegraph_copy->check();
}

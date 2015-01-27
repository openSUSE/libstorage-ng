
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    // TODO
    Actiongraph::simple_t expected = {
	{ "Create ext4 on /dev/sdb1 (7.65 GiB)", { } }
    };

    storage::Environment environment(true, ProbeMode::PROBE_READ_DEVICE_GRAPH, TargetMode::TARGET_NORMAL);
    environment.set_devicegraph_filename("test1-probed.xml");

    Storage storage(environment);

    storage.get_current()->load("test1-current.xml");

    Actiongraph actiongraph(storage, storage.get_probed(), storage.get_current());

    BOOST_CHECK_EQUAL(actiongraph.get_simple(), expected);
}

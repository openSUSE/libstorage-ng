
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Storage.h"
#include "storage/DeviceGraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    Environment environment(true, ProbeMode::PROBE_READ_DEVICE_GRAPH, TargetMode::TARGET_NORMAL);
    environment.setDeviceGraphFilename("probe.info");

    Storage storage(environment);

    BOOST_CHECK_EQUAL(storage.getDeviceGraphNames().size(), 2);

    const DeviceGraph* probed = storage.getProbed();

    probed->check();
    BOOST_CHECK_EQUAL(probed->numDevices(), 2);
    BOOST_CHECK_EQUAL(probed->numHolders(), 1);

    DeviceGraph* current = storage.getCurrent();

    current->check();
    BOOST_CHECK_EQUAL(current->numDevices(), 2);
    BOOST_CHECK_EQUAL(current->numHolders(), 1);
}

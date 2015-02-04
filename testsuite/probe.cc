
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    Environment environment(true, ProbeMode::PROBE_READ_DEVICEGRAPH, TargetMode::TARGET_NORMAL);
    environment.set_devicegraph_filename("probe.xml");

    Storage storage(environment);
    storage.check();

    BOOST_CHECK_EQUAL(storage.get_devicegraph_names().size(), 2);

    const Devicegraph* probed = storage.get_probed();

    BOOST_CHECK_EQUAL(probed->num_devices(), 2);
    BOOST_CHECK_EQUAL(probed->num_holders(), 1);

    Devicegraph* staging = storage.get_staging();

    BOOST_CHECK_EQUAL(staging->num_devices(), 2);
    BOOST_CHECK_EQUAL(staging->num_holders(), 1);
}

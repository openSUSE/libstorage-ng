
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/UsedFeatures.h"

#include "testsuite/helpers/TsCmp.h"


using namespace std;
using namespace storage;


/**
 * In this test we have a device with the stupid name 'cr #1' and a mount point with the
 * stupid name 'test #1'. Both the ' ' and the '#' need special handling.
 */
BOOST_AUTO_TEST_CASE(probe)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("luks4-mockup.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("luks4-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_EQUAL(required_features(probed), "btrfs luks swap xfs");
    BOOST_CHECK_EQUAL(suggested_features(probed), "btrfs luks swap xfs");
}

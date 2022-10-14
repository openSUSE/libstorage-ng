
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


BOOST_AUTO_TEST_CASE(probe)
{
    setenv("LIBSTORAGE_OS_FLAVOUR", "suse", 1);

    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("multipath1-mockup.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("multipath1-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_EQUAL(required_features(probed), "dasd ext2 ext4 fc multipath swap");
    BOOST_CHECK_EQUAL(suggested_features(probed), "dasd ext2 ext3 ext4 fc multipath swap");

    BOOST_CHECK_EQUAL(pools(&storage), "DASDs ECKD CDL (4 KiB) [1], MPs (512 B) [2]");
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"
#include "storage/UsedFeatures.h"

#include "testsuite/helpers/TsCmp.h"


using namespace std;
using namespace storage;


/**
 * Probe a btrfs with multiple devices. The btrfs has 4 block devices
 * and the ones listed in /etc/fstab and /proc/mounts are some from
 * the "middle", esp. not the "first".
 */
BOOST_AUTO_TEST_CASE(probe)
{
    setenv("YAST_MULTIPLE_DEVICES_BTRFS", "yes", 1);

    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("btrfs2-mockup.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("btrfs2-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_BITWISE_EQUAL(probed->used_features(), UF_EXT4 | UF_BTRFS | UF_SWAP);
}

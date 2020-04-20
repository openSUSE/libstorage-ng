
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"
#include "storage/UsedFeatures.h"

#include "testsuite/helpers/TsCmp.h"
#include "testsuite/helpers/CallbacksRecorder.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(probe)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("lvm-unsupported1-mockup.xml");

    vector<string> probe_messages;
    ProbeCallbacksRecorder probe_callbacks_recorder(probe_messages);

    Storage storage(environment);
    storage.probe(&probe_callbacks_recorder);

    BOOST_REQUIRE_EQUAL(probe_messages.size(), 3);
    BOOST_CHECK_EQUAL(probe_messages[0], "begin:");
    BOOST_CHECK_EQUAL(probe_messages[1], "error: message = 'Detected LVM logical volumes of unsupported types:\n\n"
		      "/dev/test/normal1\n/dev/test/normal1-snapshot\n\n"
		      "These logical volumes are ignored. Operations on the\ncorreponding volume "
		      "groups may fail.', what = ''");
    BOOST_CHECK_EQUAL(probe_messages[2], "end:");

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("lvm-unsupported1-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_BITWISE_EQUAL(probed->used_features(), (uint64_t)(UF_LVM | UF_EXT4));
}

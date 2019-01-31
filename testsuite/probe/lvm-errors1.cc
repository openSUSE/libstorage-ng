
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
    environment.set_mockup_filename("lvm-errors1-mockup.xml");

    Storage storage(environment);
    storage.probe();

    vector<string> check_messages;
    CheckCallbacksRecorder check_callbacks_recorder(check_messages);

    const Devicegraph* probed = storage.get_probed();
    probed->check(&check_callbacks_recorder);

    BOOST_CHECK_EQUAL(check_messages.size(), 2);
    BOOST_CHECK_EQUAL(check_messages[0], "Physical volume lv08LY-exMW-YKNh-GgX7-q0Ja-dW8H-lJ9Ejw is broken.");
    BOOST_CHECK_EQUAL(check_messages[1], "Physical volume u6c690-dm2j-IC7t-v8Lg-3Ylk-sUXY-BHsVmY is broken.");

    Devicegraph* staging = storage.get_staging();
    staging->load("lvm-errors1-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);

    BOOST_CHECK_BITWISE_EQUAL(probed->used_features(), (uint64_t)(UF_LVM));
}

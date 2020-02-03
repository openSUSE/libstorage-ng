
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"

#include "testsuite/helpers/CallbacksRecorder.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(probe)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("missing1-mockup.xml");

    vector<string> probe_messages;
    ProbeCallbacksRecorder probe_callbacks_recorder(probe_messages);

    Storage storage(environment);
    storage.probe(&probe_callbacks_recorder);

    BOOST_REQUIRE_EQUAL(probe_messages.size(), 1);
    BOOST_CHECK_EQUAL(probe_messages[0], "missing-command: message = 'Probing file systems failed', "
		      "what = 'Command not found: \"/sbin/btrfs filesystem show\"', "
		      "command = '/sbin/btrfs filesystem show', used_features = 8");
}

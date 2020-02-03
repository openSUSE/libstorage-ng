
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
    environment.set_mockup_filename("error1-mockup.xml");

    vector<string> probe_messages;
    ProbeCallbacksRecorder probe_callbacks_recorder(probe_messages);

    Storage storage(environment);
    storage.probe(&probe_callbacks_recorder);

    BOOST_REQUIRE_EQUAL(probe_messages.size(), 1);
    BOOST_CHECK_EQUAL(probe_messages[0], "error: message = 'Probing LVM failed', what = 'command '/sbin/pvs "
		      "--reportformat json --units b --nosuffix --all --options pv_name,pv_uuid,vg_name,vg_uuid,"
		      "pv_attr,pe_start' failed:\n\nstderr:\nerror: system is screwed\n\nexit code:\n42'");
}

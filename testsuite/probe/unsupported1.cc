
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/UsedFeatures.h"

#include "testsuite/helpers/TsCmp.h"
#include "testsuite/helpers/CallbacksRecorder.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(probe)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("unsupported1-mockup.xml");

    vector<string> probe_messages;
    ProbeCallbacksRecorder probe_callbacks_recorder(probe_messages);

    Storage storage(environment);
    storage.probe(&probe_callbacks_recorder);

    BOOST_REQUIRE_EQUAL(probe_messages.size(), 4);
    BOOST_CHECK_EQUAL(probe_messages[0], "begin:");
    BOOST_CHECK_EQUAL(probe_messages[1], "unsupported-partition-table: message = 'Detected an unsupported partition "
		      "table of type Amiga on the\ndevice /dev/sdd.', what = '', name = '/dev/sdd', pt-type = Amiga");
    BOOST_CHECK_EQUAL(probe_messages[2], "unsupported-filesystem: message = 'Detected an unsupported file system of "
		      "type minix on the\ndevice /dev/sdc1.', what = '', name = '/dev/sdc1', fs-type = minix");
    BOOST_CHECK_EQUAL(probe_messages[3], "end:");

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    Devicegraph* staging = storage.get_staging();
    staging->load("unsupported1-devicegraph.xml");
    staging->check();

    TsCmpDevicegraph cmp(*probed, *staging);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

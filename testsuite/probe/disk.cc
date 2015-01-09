
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    Environment environment(true, ProbeMode::PROBE_READ_MOCKUP, TargetMode::TARGET_NORMAL);
    environment.set_mockup_filename("disk-mockup.xml");

    Storage storage(environment);

    const Devicegraph* probed = storage.get_probed();

    probed->check();

    Devicegraph* current = storage.get_current();

    current->clear();
    current->load("disk-devicegraph.xml");
    current->check();

    // TODO compare probed and current
}

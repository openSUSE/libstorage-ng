
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    storage::Environment environment(true, ProbeMode::PROBE_READ_MOCKUP, TargetMode::TARGET_NORMAL);
    environment.set_mockup_filename("disk-mockup.xml");

    Storage storage(environment);

    const Devicegraph* probed = storage.get_probed();

    probed->check();

    Devicegraph* current = storage.get_current();

    current->load("disk-devicegraph.xml");
    current->check();

    bool equal = *probed == *current;

    BOOST_CHECK_MESSAGE(equal, "not equal");

    if (!equal)
	probed->get_impl().log_diff(cout, current->get_impl());
}

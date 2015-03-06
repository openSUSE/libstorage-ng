
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    Cmp::expected_t expected = {
	{ "1 - Create ext4 on /dev/sdb1 (7.65 GiB) ->" }
    };

    storage::Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("test1-probed.xml");

    Storage storage(environment);

    storage.get_staging()->load("test1-staging.xml");

    Actiongraph actiongraph(storage, storage.get_probed(), storage.get_staging());

    Cmp cmp(actiongraph, expected);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

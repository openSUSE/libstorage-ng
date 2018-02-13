
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(is_delete, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_is_delete)
{
    initialize_with_devicegraph("devicegraph.xml");

    auto partition_name = "/dev/sda1";

    delete_partition(partition_name);

    auto actiongraph = storage->calculate_actiongraph();

    auto deleted_partition = Partition::find_by_name(storage->get_probed(), partition_name);

    auto compound_action = find_compound_action_by_target(actiongraph, deleted_partition);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK(compound_action->is_delete());
}


BOOST_AUTO_TEST_CASE(test_is_not_delete)
{
    initialize_staging_with_two_partitions();

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda1);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK(!compound_action->is_delete());
}


BOOST_AUTO_TEST_SUITE_END()

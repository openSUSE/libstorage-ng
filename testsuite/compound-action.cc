
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include "memory"
#include "string"

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Partition.h"

using namespace std;
using namespace storage;


struct Fixture
{
    Fixture() :
    probed_xml("compound-action-probed.xml"),
    staging_xml("compound-action-staging.xml")
    {
	set_probed();
    }

    void
    set_probed()
    {
	Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
	environment.set_devicegraph_filename(probed_xml);
	
	storage = shared_ptr<Storage>(new Storage(environment));
	storage->probe();
    }

    void
    set_staging()
    {
	auto staging = storage->get_staging();
	staging->load(staging_xml);
	staging->check();
    }

    static const CompoundAction*
    find_compound_action_by_target(const vector<const CompoundAction*>& actions, const Device* device)
    {
	for (auto action : actions)
	{
	    if (action->get_target_device() == device)
		return action;
	}

	return nullptr;
    }

    shared_ptr<Storage> storage;
    string probed_xml;
    string staging_xml;
};


BOOST_FIXTURE_TEST_SUITE(compound_actions, Fixture)

BOOST_AUTO_TEST_CASE(test_size_without_commit_actions)
{
    auto actiongraph = storage->calculate_actiongraph();

    BOOST_CHECK(actiongraph->get_commit_actions().size() == 0);

    BOOST_CHECK(actiongraph->get_compound_actions().size() == 0);
}


BOOST_AUTO_TEST_CASE(test_size_with_commit_actions)
{
    set_staging();

    auto actiongraph = storage->calculate_actiongraph();

    BOOST_CHECK(actiongraph->get_commit_actions().size() == 182);

    BOOST_CHECK(actiongraph->get_compound_actions().size() == 74);
}


BOOST_AUTO_TEST_CASE(test_is_delete)
{
    set_staging();

    auto actiongraph = storage->calculate_actiongraph();

    auto sda2 = Partition::find_by_name(storage->get_probed(), "/dev/sda2");
    BOOST_REQUIRE(sda2);

    auto compound_action = find_compound_action_by_target(actiongraph->get_compound_actions(), sda2);
    BOOST_REQUIRE(compound_action);

    BOOST_CHECK(compound_action->is_delete());
}


BOOST_AUTO_TEST_CASE(test_is_not_delete)
{
    set_staging();

    auto actiongraph = storage->calculate_actiongraph();

    auto sda1 = Partition::find_by_name(storage->get_staging(), "/dev/sda1");
    BOOST_REQUIRE(sda1);

    auto compound_action = find_compound_action_by_target(actiongraph->get_compound_actions(), sda1);
    BOOST_REQUIRE(compound_action);

    BOOST_CHECK(!compound_action->is_delete());
}

BOOST_AUTO_TEST_SUITE_END()


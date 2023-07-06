
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Actions/Base.h"
#include "storage/Utils/Logger.h"

using namespace storage;
using namespace std;


/**
 * Check that swap is activated ASAP.
 */
BOOST_AUTO_TEST_CASE(order)
{
    setenv("LIBSTORAGE_TOPOLOGICAL_SORT_METHOD", "1", 1);

    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("swap1-probed.xml");

    Storage storage(environment);
    storage.probe();
    storage.get_staging()->load("swap1-staging.xml");

    const Actiongraph* actiongraph = storage.calculate_actiongraph();

    const vector<const Action::Base*> actions = actiongraph->get_commit_actions();

    BOOST_REQUIRE(actions.size() >= 5);

    BOOST_CHECK_EQUAL(get_string(actiongraph, actions[0]), "Create GPT on /dev/nvme0n1");
    BOOST_CHECK_EQUAL(get_string(actiongraph, actions[1]), "Create partition /dev/nvme0n1p1 (40.00 GiB)");
    BOOST_CHECK_EQUAL(get_string(actiongraph, actions[2]), "Create partition /dev/nvme0n1p2 (2.00 GiB)");
    BOOST_CHECK_EQUAL(get_string(actiongraph, actions[3]), "Create swap on /dev/nvme0n1p2 (2.00 GiB)");
    BOOST_CHECK_EQUAL(get_string(actiongraph, actions[4]), "Mount /dev/nvme0n1p2 (2.00 GiB) at swap");
}

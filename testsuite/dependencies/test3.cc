
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <unistd.h>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph::Expected expected("test3-expected.txt");

    storage::Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("test3-probed.xml");

    Storage storage(environment);

    storage.get_staging()->load("test3-staging.xml");

    Actiongraph actiongraph(storage, storage.get_probed(), storage.get_staging());
    if (access("/usr/bin/dot", X_OK) == 0)
    {
	actiongraph.write_graphviz("test3.gv", true);
	system("dot -Tpng < test3.gv > test3.png");
    }

    TsCmpActiongraph cmp(actiongraph.get_impl(), expected);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

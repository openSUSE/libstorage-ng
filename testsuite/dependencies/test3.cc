
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
    TsCmpActiongraph::expected_t expected;

    std::ifstream fin("test3-expected.txt");
    string line;
    while (getline(fin, line))
    {
	if (!line.empty() && !boost::starts_with(line, "#"))
	    expected.push_back(line);
    }

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

    TsCmpActiongraph cmp(actiongraph, expected);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

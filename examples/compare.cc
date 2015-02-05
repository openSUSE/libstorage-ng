

#include <stdlib.h>
#include <iostream>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Actiongraph.h"


using namespace std;
using namespace storage;


int
main()
{
    storage::Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* lhs = storage.create_devicegraph("lhs");
    cout << "loading lhs" << endl;
    lhs->load("compare-probed.xml");
    if (false)
    {
	lhs->get_impl().write_graphviz("compare-probed.gv", true);
	system("dot -Tpng < compare-probed.gv > compare-probed.png");
	system("display compare-probed.png");
    }

    Devicegraph* rhs = storage.create_devicegraph("rhs");
    cout << "loading rhs" << endl;
    rhs->load("compare-staging.xml");
    if (false)
    {
	rhs->get_impl().write_graphviz("compare-staging.gv", true);
	system("dot -Tpng < compare-staging.gv > compare-staging.png");
	system("display compare-staging.png");
    }

    cout << "calculating actions" << endl;
    Actiongraph actiongraph(storage, lhs, rhs);
    if (true)
    {
	actiongraph.write_graphviz("compare-action.gv", true);
	system("dot -Tpng < compare-action.gv > compare-action.png");
	system("display compare-action.png");
    }
}

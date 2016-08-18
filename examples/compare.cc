

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
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* lhs = storage.create_devicegraph("lhs");
    cout << "loading lhs" << endl;
    lhs->load("compare-probed.xml");
    if (false)
    {
	lhs->get_impl().write_graphviz("compare-probed.gv", GraphvizFlags::SID);
	system("dot -Tsvg < compare-probed.gv > compare-probed.svg");
	system("display compare-probed.svg");
    }

    Devicegraph* rhs = storage.create_devicegraph("rhs");
    cout << "loading rhs" << endl;
    rhs->load("compare-staging.xml");
    if (false)
    {
	rhs->get_impl().write_graphviz("compare-staging.gv", GraphvizFlags::SID);
	system("dot -Tsvg < compare-staging.gv > compare-staging.svg");
	system("display compare-staging.svg");
    }

    cout << "calculating actions" << endl;
    Actiongraph actiongraph(storage, lhs, rhs);
    if (true)
    {
	actiongraph.write_graphviz("compare-action.gv", GraphvizFlags::SID);
	system("dot -Tsvg < compare-action.gv > compare-action.svg");
	system("display compare-action.svg");
    }
}



#include <stdlib.h>
#include <iostream>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
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

    Devicegraph* rhs = storage.create_devicegraph("rhs");
    cout << "loading rhs" << endl;
    rhs->load("compare-staging.xml");

    cout << "calculating actions" << endl;
    Actiongraph actiongraph(storage, lhs, rhs);
    actiongraph.write_graphviz("compare-action.gv");
    system("dot -Tpng < compare-action.gv > compare-action.png");
    system("display compare-action.png");
}


#include <iostream>

#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devicegraph.h"


using namespace std;
using namespace storage;


int
main()
{
    Environment environment(true, ProbeMode::STANDARD_WRITE_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("probe1-mockup.xml");

    Storage storage(environment);

    const Devicegraph* probed = storage.get_probed();

    cout << *probed << endl;

    probed->check();
    probed->write_graphviz("probe1.gv");
    probed->save("probe1-devicegraph.xml");
}

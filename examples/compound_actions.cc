#include <iostream>

#include "storage/Utils/Logger.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/ActiongraphImpl.h"
#include "storage/Action.h"
#include "storage/CompoundAction.h"
#include "storage/CompoundActionImpl.h"
#include "storage/UsedFeatures.h"

using namespace std;
using namespace storage;


int
main()
{
    // Log into file (/var/log/libstorage-ng.log).
    set_logger(get_logfile_logger());

    try
    {
	// Create storage object and probe system.
	Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
	environment.set_devicegraph_filename("examples/data/compound-actions-probed.xml");

	Storage storage(environment);
	storage.probe();

	const Devicegraph* probed = storage.get_probed();

	probed->check();

	Devicegraph* staging = storage.get_staging();

	staging->load("examples/data/compound-actions-staging.xml");
	staging->check();

	// Calculate the actiongraph.
	const Actiongraph* actiongraph = storage.calculate_actiongraph();

	cout << "Number of commit actions:" << actiongraph->get_commit_actions().size() << endl;

	for (const string& text : actiongraph->get_commit_actions_as_strings())
	    cout << text << endl;

	const vector<const CompoundAction*> compound_actions = actiongraph->get_compound_actions();

	cout << "Number of compound actions:" << compound_actions.size() << endl;

	for (const CompoundAction* compound_action : compound_actions)
	{
	    cout << "--> Compound action (" << compound_action->get_impl().get_commit_actions().size() << ")" << endl;
	    cout << compound_action->sentence() << endl;
	}

	return EXIT_SUCCESS;
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << endl;

	return EXIT_FAILURE;
    }
}

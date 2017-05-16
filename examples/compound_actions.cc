#include <iostream>

#include "storage/Utils/Logger.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
//#include "storage/DevicegraphImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Action.h"
#include "storage/CompoundAction/CompoundAction.h"
#include "storage/CompoundAction/CompoundActionImpl.h"
//#include "storage/CompoundAction/CompoundActionGenerator.h"
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
	environment.set_devicegraph_filename("examples/data/empty_hard_disk_50GiB.xml");
	//environment.set_devicegraph_filename("examples/data/mydisk_probed.xml");

	Storage storage(environment);
	storage.probe();

	const Devicegraph* probed = storage.get_probed();

	probed->check();

	Devicegraph* staging = storage.get_staging();

	staging->load("examples/data/proposal_from_empty_hard_disk_50GiB.xml");
	//staging->load("examples/data/mydisk_staging.xml");
	staging->check();

	// Calculate the actiongraph.
	const Actiongraph* actiongraph = storage.calculate_actiongraph();

	cout << "Number of commit actions:" << actiongraph->get_commit_actions().size() << endl;

	for(auto text : actiongraph->get_commit_actions_as_strings())
	    cout << text << endl;



//	for (auto action : actiongraph->get_commit_actions())
//	{
//	    cout << action->sid << "/" << CompoundAction::Impl::get_target_device(actiongraph, action)->get_sid() << endl;
//	}

	//CompoundActionGenerator generator(actiongraph);

	auto compound_actions = CompoundAction::generate(actiongraph);

	cout << "Number of compound actions:" << compound_actions.size() << endl;

	for (auto& compound_action : compound_actions)
	{
	    cout << "--> Compound action" << endl;
	    cout << compound_action->string_representation() << endl;
	    //for (auto action : compound_action->get_impl().get_commit_actions_as_strings())
	    //	cout << action << endl;
	}
	

	for (auto& compound_action : compound_actions)
	    delete compound_action;

	return EXIT_SUCCESS;
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << endl;

	return EXIT_FAILURE;
    }
}


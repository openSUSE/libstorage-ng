#ifndef TEST_COMPOUND_ACTION_FIXTURE_H
#define TEST_COMPOUND_ACTION_FIXTURE_H

#include <memory>
#include <vector>
#include <string>

#include "storage/Utils/HumanString.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/CompoundAction.h"

using namespace std;


namespace storage
{
    namespace test
    {
	struct CompoundActionFixture
	{

	    void
	    initialize_staging_with_one_disk()
	    {
		Environment environment(true, ProbeMode::NONE, TargetMode::IMAGE);
		storage = make_unique<Storage>(environment);
		staging = storage->get_staging();

		sda = Disk::create(staging, "/dev/sda");
		sda_gpt = to_gpt(sda->create_partition_table(PtType::GPT));
	    }


	    void
	    initialize_staging_with_three_partitions()
	    {
		initialize_staging_with_one_disk();

		sda1 = sda_gpt->create_partition("/dev/sda1", Region(  1 * 2048,   4 * 2048, 512), PartitionType::PRIMARY);
		sda2 = sda_gpt->create_partition("/dev/sda2", Region(  5 * 2048, 500 * 2048, 512), PartitionType::PRIMARY);
		sda3 = sda_gpt->create_partition("/dev/sda3", Region(505 * 2048, 500 * 2048, 512), PartitionType::PRIMARY);
	    }


	    void
	    initialize_with_devicegraph(string devicegraph_file)
	    {
		Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
		environment.set_devicegraph_filename(devicegraph_file);

		storage = make_unique<Storage>(environment);
		storage->probe();

		auto probed = storage->get_probed();
		probed->check();

		staging = storage->get_staging();
	    }


	    void
	    delete_partition(const string partition_name)
	    {
		auto partition = Partition::find_by_name(staging, partition_name);
		auto partition_table = const_cast<PartitionTable*>(partition->get_partition_table());
		partition_table->delete_partition(partition);
	    }


	    static const CompoundAction*
	    find_compound_action_by_target(const Actiongraph* actiongraph, const Device* device)
	    {
		for (auto action : actiongraph->get_compound_actions())
		{
		    if (action->get_target_device() == device)
			return action;
		}

		return nullptr;
	    }


            void
            copy_staging_to_probed()
            {
                storage->remove_devicegraph("system");
                storage->copy_devicegraph("staging", "system");
            }


	    unique_ptr<Storage> storage;
	    Devicegraph* staging = nullptr;

	    Disk* sda = nullptr;
	    Gpt* sda_gpt = nullptr;
	    Partition* sda1 = nullptr;
	    Partition* sda2 = nullptr;
	    Partition* sda3 = nullptr;

	};

    }

}


#endif

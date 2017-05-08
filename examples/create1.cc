

#include <iostream>

#include "storage/Utils/Logger.h"
#include "storage/Utils/HumanString.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/Swap.h"


using namespace std;
using namespace storage;


void
create(Storage& storage)
{
    // Get the staging devicegraph.
    Devicegraph* staging = storage.get_staging();

    // Find disk "/dev/sdb".
    Disk* sdb = Disk::find_by_name(staging, "/dev/sdb");

    // Create a GPT on "sdb".
    PartitionTable* gpt = sdb->create_partition_table(PtType::GPT);

    // Create partition "sdb1" with 16 GiB size and set id to LVM. This is
    // done by querying free slots (there is only one since there are no
    // partitions yet) and then setting the size of the region. This method
    // ensures the partition is properly aligned for best performance.
    PartitionSlot partition_slot = gpt->get_unused_partition_slots()[0];
    partition_slot.region.set_length(partition_slot.region.to_blocks(16 * GiB));
    Partition* sdb1 = gpt->create_partition(partition_slot.name, partition_slot.region,
					    PartitionType::PRIMARY);
    sdb1->set_id(ID_LVM);

    // Create volume group "test" using "sdb1" as physical volume.
    LvmVg* test_vg = LvmVg::create(staging, "test");
    test_vg->add_lvm_pv(sdb1);

    // Create logical volume "data" with ext4 on it.
    LvmLv* data_lv = test_vg->create_lvm_lv("data", 8 * GiB);
    Ext4* ext4_fs = to_ext4(data_lv->create_blk_filesystem(FsType::EXT4));

    // Create mount point for ext4.
    ext4_fs->create_mount_point("/test");

    // Create logical volume "swap" with swap on it.
    LvmLv* swap_lv = test_vg->create_lvm_lv("swap", 1 * GiB);
    Swap* swap_fs = to_swap(swap_lv->create_blk_filesystem(FsType::SWAP));

    // Create mount point for swap.
    swap_fs->create_mount_point("swap");

    // Check the staging devicegraph.
    staging->check();
}


class MyCommitCallbacks : public CommitCallbacks
{
public:

    virtual ~MyCommitCallbacks() {}

    virtual void message(const string& message) const override
    {
	cout << message << endl;
    }

    virtual bool error(const string& message, const string& what) const override
    {
	cerr << message << ' ' << what << endl;
	return false;
    }
};


void
commit(Storage& storage)
{
    // Calculate the actiongraph.
    const Actiongraph* actiongraph = storage.calculate_actiongraph();

#if 1

    // Print commit actions.
    for (const string& text : actiongraph->get_commit_actions_as_strings())
	cout << text << endl;

#else

    // Commit changes to disk.
    MyCommitCallbacks my_commit_callbacks;
    storage.commit(&my_commit_callbacks);

#endif
}


int
main()
{
    // Log into file (/var/log/libstorage-ng.log).
    set_logger(get_logfile_logger());

    try
    {
	// Create storage object and probe system.
	Environment environment(false, ProbeMode::STANDARD, TargetMode::DIRECT);
	Storage storage(environment);
	storage.probe();

	create(storage);

	commit(storage);

	return EXIT_SUCCESS;
    }
    catch (const exception& e)
    {
	cerr << "exception occured: " << e.what() << endl;

	return EXIT_FAILURE;
    }
}

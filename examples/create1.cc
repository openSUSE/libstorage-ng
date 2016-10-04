

#include <iostream>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Utils/Logger.h"
#include "storage/Utils/HumanString.h"
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
    // Get the stating devicegraph
    Devicegraph* staging = storage.get_staging();

    // Find disk "/dev/sdb"
    Disk* sdb = Disk::find_by_name(staging, "/dev/sdb");

    // Create a GPT on "sdb"
    PartitionTable* gpt = sdb->create_partition_table(PtType::GPT);

    // Create partition "sdb1" with 16 GiB size and set id to LVM
    unsigned long long block_size = sdb->get_region().get_block_size();
    unsigned long start = 2048;
    unsigned long length = 16 * GiB / block_size;

    Partition* sdb1 = gpt->create_partition("/dev/sdb1", Region(start, length, block_size),
					    PartitionType::PRIMARY);
    sdb1->set_id(ID_LVM);

    // Create LVM volume group "test" using "sdb1" as physical volume
    LvmVg* test = LvmVg::create(staging, "test");
    test->add_lvm_pv(sdb1);

    // Create logical volume "foo" with ext4 on it
    LvmLv* foo = test->create_lvm_lv("foo", 8 * GiB);
    Ext4* ext4 = to_ext4(foo->create_filesystem(FsType::EXT4));
    ext4->add_mountpoint("/test");

    // Create logical volume "bar" with swap on it
    LvmLv* bar = test->create_lvm_lv("bar", 1 * GiB);
    Swap* swap = to_swap(bar->create_filesystem(FsType::SWAP));
    swap->add_mountpoint("swap");
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
    // Calculate the actiongraph
    const Actiongraph* actiongraph = storage.calculate_actiongraph();

#if 1

    // Print commit actions
    for (const string& text : actiongraph->get_commit_actions_as_strings())
	cout << text << endl;

#else

    // Commit changes to disk
    MyCommitCallbacks my_commit_callbacks;
    storage.commit(&my_commit_callbacks);

#endif
}


int
main()
{
    set_logger(get_logfile_logger());

    Environment environment(false, ProbeMode::STANDARD, TargetMode::DIRECT);

    Storage storage(environment);

    create(storage);

    commit(storage);
}

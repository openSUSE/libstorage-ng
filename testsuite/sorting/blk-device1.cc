
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/BackedBcache.h"
#include "storage/Devices/StrayBlkDevice.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<BlkDevice*>& blk_devices)
    {
	s << "{";
	for (vector<BlkDevice*>::const_iterator it = blk_devices.begin(); it != blk_devices.end(); ++it)
	    s << (it == blk_devices.begin() ? " " : ", ") << (*it)->get_displayname();
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(blk_device_sorting1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    PartitionTable* sda_pt = sda->create_partition_table(PtType::GPT);
    Partition* sda1 = sda_pt->create_partition("/dev/sda1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* sda10 = sda_pt->create_partition("/dev/sda10", Region(101 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* sda2 = sda_pt->create_partition("/dev/sda2", Region(201 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);

    Disk* sdb = Disk::create(staging, "/dev/sdb");
    PartitionTable* sdb_pt = sdb->create_partition_table(PtType::GPT);
    Partition* sdb1 = sdb_pt->create_partition("/dev/sdb1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* sdb10 = sdb_pt->create_partition("/dev/sdb10", Region(101 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* sdb2 = sdb_pt->create_partition("/dev/sdb2", Region(201 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);

    Disk* xvdb = Disk::create(staging, "/dev/xvdb");
    PartitionTable* xvdb_pt = xvdb->create_partition_table(PtType::GPT);
    Partition* xvdb1 = xvdb_pt->create_partition("/dev/xvdb1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* xvdb10 = xvdb_pt->create_partition("/dev/xvdb10", Region(101 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* xvdb2 = xvdb_pt->create_partition("/dev/xvdb2", Region(201 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);

    Md* md_a10 = Md::create(staging, "/dev/md/a10");
    Md* md_a2 = Md::create(staging, "/dev/md/a2");

    Md* md_b = Md::create(staging, "/dev/md/b");
    PartitionTable* md_b_pt = md_b->create_partition_table(PtType::GPT);
    Partition* md_b1 = md_b_pt->create_partition("/dev/md/b1", Region(1 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* md_b10 = md_b_pt->create_partition("/dev/md/b10", Region(101 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);
    Partition* md_b2 = md_b_pt->create_partition("/dev/md/b2", Region(201 * 2048, 100 * 2048, 512), PartitionType::PRIMARY);

    LvmVg* test = LvmVg::create(staging, "test");
    LvmLv* test_aa = test->create_lvm_lv("aa", LvType::NORMAL, 1 * GiB);
    LvmLv* test_b = test->create_lvm_lv("b", LvType::NORMAL, 1 * GiB);

    Bcache* bcache2 = BackedBcache::create(staging, "/dev/bcache2");
    Bcache* bcache10 = BackedBcache::create(staging, "/dev/bcache10");

    StrayBlkDevice* xvdc2 = StrayBlkDevice::create(staging, "/dev/xvdc2");
    StrayBlkDevice* xvdc10 = StrayBlkDevice::create(staging, "/dev/xvdc10");

    vector<BlkDevice*> all = BlkDevice::get_all(staging);
    sort(all.begin(), all.end(), BlkDevice::compare_by_name);

    BOOST_CHECK_EQUAL(all, vector<BlkDevice*>({
	bcache2, bcache10,
	md_a10, md_a2,			// sorted alphabetically
	md_b, md_b1, md_b2, md_b10,	// sorted numerically
	sda, sda1, sda2, sda10,
	sdb, sdb1, sdb2, sdb10,
	test_aa, test_b,
	xvdb, xvdb1, xvdb2, xvdb10,
	xvdc2, xvdc10
    }));
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Holders/User.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "testsuite/helpers/TsCmp.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(dependencies)
{
    set_logger(get_stdout_logger());

    TsCmpActiongraph::Expected expected({
	{ "1 - Create GPT on /dev/sda -> 2a" },
	{ "2a - Create partition /dev/sda1 (16.00 GiB) -> 2b" },
	{ "2b - Set id of partition /dev/sda1 to Linux LVM -> 3" },
	{ "3 - Create physical volume on /dev/sda1 -> 4" },
	{ "4 - Create volume group system (16.00 GiB) from /dev/sda1 (16.00 GiB) -> 5 6" },
	{ "5 - Create logical volume root (14.00 GiB) on volume group system ->" },
	{ "6 - Create logical volume swap (2.00 GiB) on volume group system ->" }
    });

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* lhs = storage.get_staging();

    Disk* tmp = Disk::create(lhs, "/dev/sda");
    tmp->set_region(Region(0, 100000, 512));

    Devicegraph* rhs = storage.copy_devicegraph("staging", "old");

    Disk* sda = to_disk(BlkDevice::find_by_name(rhs, "/dev/sda"));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = Partition::create(rhs, "/dev/sda1", Region(0, 0, 512), PartitionType::PRIMARY);
    Subdevice::create(rhs, gpt, sda1);
    sda1->set_size(16 * GiB);
    sda1->set_id(ID_LVM);

    LvmVg* system = LvmVg::create(rhs, "system");
    system->add_lvm_pv(sda1);

    LvmLv* system_root = system->create_lvm_lv("root", LvType::NORMAL, 4 * GiB);
    system_root->set_size(14 * GiB);

    LvmLv* system_swap = system->create_lvm_lv("swap", LvType::NORMAL, 1 * GiB);
    system_swap->set_size(2 * GiB);

    Actiongraph actiongraph(storage, lhs, rhs);

    TsCmpActiongraph cmp(actiongraph, expected);
    BOOST_CHECK_MESSAGE(cmp.ok(), cmp);
}

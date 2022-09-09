
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Luks.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


const unsigned long long mpg = MiB / 512;	// sectors per MiB
const unsigned long long spg = GiB / 512;	// sectors per GiB


BOOST_AUTO_TEST_CASE(test_luks)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 200 * spg, 512));
    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);
    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1 * mpg, 10 * spg, 512), PartitionType::PRIMARY);
    Luks* luks = to_luks(sda1->create_encryption("cr-test", EncryptionType::LUKS1));
    luks->create_blk_filesystem(FsType::EXT4);

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    BOOST_CHECK_EQUAL(sda1->get_size_string(), "10.00 GiB");
    BOOST_CHECK_EQUAL(luks->get_size_string(), "10.00 GiB");

    sda1->set_size(30 * GiB);

    BOOST_CHECK_EQUAL(sda1->get_size_string(), "30.00 GiB");
    BOOST_CHECK_EQUAL(luks->get_size_string(), "30.00 GiB");

    sda1->set_size(20 * GiB);

    BOOST_CHECK_EQUAL(sda1->get_size_string(), "20.00 GiB");
    BOOST_CHECK_EQUAL(luks->get_size_string(), "20.00 GiB");
}

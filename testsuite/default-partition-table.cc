
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/Arch.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_size_k(320 * 1024 * 1024); // 320 GiB

    BOOST_CHECK(sda->get_default_partition_table_type() == PtType::MSDOS);
}


BOOST_AUTO_TEST_CASE(test2)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_size_k(3ULL * 1024 * 1024 * 1024); // 3 TiB

    BOOST_CHECK(sda->get_default_partition_table_type() == PtType::GPT);
}


BOOST_AUTO_TEST_CASE(test3)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);
    storage.get_impl().get_arch().set_efiboot(true);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_size_k(320 * 1024 * 1024); // 320 GiB

    BOOST_CHECK(sda->get_default_partition_table_type() == PtType::GPT);
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage


#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/GptImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_msdos)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 2097152, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    BOOST_CHECK(msdos->get_impl().has_usable_region());
    BOOST_CHECK_EQUAL(msdos->get_impl().get_usable_region().get_start(), 2048);
    BOOST_CHECK_EQUAL(msdos->get_impl().get_usable_region().get_end(), 2097151);
}


BOOST_AUTO_TEST_CASE(test_msdos_small_disk)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 2048, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    BOOST_CHECK(!msdos->get_impl().has_usable_region());
    BOOST_CHECK_THROW(msdos->get_impl().get_usable_region(), Exception);
}


BOOST_AUTO_TEST_CASE(test_msdos_huge_disk)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 6442450944, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    BOOST_CHECK(msdos->get_impl().has_usable_region());
    BOOST_CHECK_EQUAL(msdos->get_impl().get_usable_region().get_start(), 2048);
    BOOST_CHECK_EQUAL(msdos->get_impl().get_usable_region().get_end(), UINT32_MAX - 1);
}

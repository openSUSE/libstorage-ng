
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"
#include "storage/FreeInfo.h"


using namespace std;
using namespace storage;


const unsigned long long spg = GiB / 512;	// sectors per GiB


BOOST_AUTO_TEST_CASE(test_msdos)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 160 * spg, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(10 * spg, 10 * spg, 512), PartitionType::PRIMARY);

    Partition* sda2 = msdos->create_partition("/dev/sda2", Region(40 * spg, 20 * spg, 512), PartitionType::PRIMARY);
    sda2->create_blk_filesystem(FsType::EXT4);

    Partition* sda3 = msdos->create_partition("/dev/sda3", Region(80 * spg, 40 * spg, 512), PartitionType::PRIMARY);
    sda3->create_blk_filesystem(FsType::SWAP);

    ResizeInfo resize_info_sda1 = sda1->detect_resize_info();
    BOOST_CHECK(resize_info_sda1.resize_ok);
    BOOST_CHECK_EQUAL(resize_info_sda1.min_size, 512);
    BOOST_CHECK_EQUAL(resize_info_sda1.max_size, 30 * GiB);

    ResizeInfo resize_info_sda2 = sda2->detect_resize_info();
    BOOST_CHECK(resize_info_sda2.resize_ok);
    BOOST_CHECK_EQUAL(resize_info_sda2.min_size, 32 * MiB);
    BOOST_CHECK_EQUAL(resize_info_sda2.max_size, 40 * GiB);

    ResizeInfo resize_info_sda3 = sda3->detect_resize_info();
    BOOST_CHECK(resize_info_sda3.resize_ok);
    BOOST_CHECK_EQUAL(resize_info_sda3.min_size, 40 * KiB);
    BOOST_CHECK_EQUAL(resize_info_sda3.max_size, 80 * GiB);
}

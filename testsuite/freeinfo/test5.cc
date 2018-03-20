
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Partition.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"
#include "storage/FreeInfo.h"


using namespace std;
using namespace storage;


const unsigned long long spm = MiB / 512;	// sectors per MiB
const unsigned long long spg = GiB / 512;	// sectors per GiB


BOOST_AUTO_TEST_CASE(test_msdos1)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 160 * spg, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(1 * spm, 1, 512), PartitionType::PRIMARY);

    Partition* sda2 = msdos->create_partition("/dev/sda2", Region(1 * spg, 1 * spg, 512), PartitionType::PRIMARY);

    Partition* sda3 = msdos->create_partition("/dev/sda3", Region(2 * spg, 1 * spg, 512), PartitionType::PRIMARY);

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    {
	ResizeInfo resize_info = sda1->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.reasons, RB_MIN_SIZE_FOR_PARTITION);
	BOOST_CHECK_EQUAL(resize_info.min_size, 512);
	BOOST_CHECK_EQUAL(resize_info.max_size, 1 * GiB - 1 * MiB);
    }

    {
	ResizeInfo resize_info = sda2->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.reasons, RB_NO_SPACE_BEHIND_PARTITION);
	BOOST_CHECK_EQUAL(resize_info.min_size, 512);
	BOOST_CHECK_EQUAL(resize_info.max_size, 1 * GiB);
    }

    {
	ResizeInfo resize_info = sda3->detect_resize_info();
	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.reasons, 0);
	BOOST_CHECK_EQUAL(resize_info.min_size, 512);
	BOOST_CHECK_EQUAL(resize_info.max_size, 158 * GiB);
    }
}

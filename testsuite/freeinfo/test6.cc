
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/FreeInfo.h"


/*
 * Some unit tests where external tools are used to retrieve the min
 * size of a filesystem.
 */


using namespace std;
using namespace storage;


const unsigned long long spg = GiB / 512;	// sectors per GiB
const unsigned long long spt = TiB / 512;	// sectors per TiB


BOOST_AUTO_TEST_CASE(test_ext4)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    // Create 40 TiB disk with one 10 TiB partition.

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 40 * spt, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1 * spg, 10 * spt, 512), PartitionType::PRIMARY);
    sda1->create_blk_filesystem(FsType::EXT4);

    // Copy staging devicegraph to system devicegraph so that querying
    // the resize-info uses external commands.

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    // Setup mocking to what querying resize-info needs.

    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::set_command(DUMPE2FS_BIN " -h '/dev/sda1'", vector<string> {
	"Filesystem features:      64bit",
	"Block size:               4096",
    });

    Mockup::set_command(RESIZE2FS_BIN " -P '/dev/sda1'", vector<string> {
	"Estimated minimum size of the filesystem: 1000000"
    });

    Mockup::set_command({ UDEVADM_BIN_SETTLE }, vector<string> {});

    // Check min and max size.

    {
	ResizeInfo resize_info = sda1->detect_resize_info();

	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 1000000 * 4 * KiB);

	// max size is disk size - start of partition - size of
	// secondary GPT round up to 4 KiB (block size of filesystem)
	BOOST_CHECK_EQUAL(resize_info.max_size, 40 * TiB - 1 * GiB - 20 * KiB);
    }
}


BOOST_AUTO_TEST_CASE(test_ntfs)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    // Create 4 TiB disk with one 1 TiB partition.

    Disk* sda = Disk::create(staging, "/dev/sda", Region(0, 4 * spt, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1 * spg, 1 * spt, 512), PartitionType::PRIMARY);
    sda1->create_blk_filesystem(FsType::NTFS);

    // Copy staging devicegraph to system devicegraph so that querying
    // the resize-info uses external commands.

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    // Setup mocking to what querying resize-info needs.

    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::set_command(NTFSRESIZE_BIN " --force --info '/dev/sda1'", vector<string> {
	"You might resize at 10737418240 bytes or 10 GB (freeing 1014 GB)."
    });

    Mockup::set_command({ UDEVADM_BIN_SETTLE }, vector<string> {});

    // Check min and max size.

    {
	ResizeInfo resize_info = sda1->detect_resize_info();

	BOOST_CHECK(resize_info.resize_ok);
	BOOST_CHECK_EQUAL(resize_info.min_size, 10 * GiB + 100 * MiB);

	// max size is disk size - start of partition - size of secondary GPT
	BOOST_CHECK_EQUAL(resize_info.max_size, 4 * TiB - 1 * GiB - 16.5 * KiB);
    }
}

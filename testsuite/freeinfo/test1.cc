
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Logger.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Devices/Partition.h"
#include "storage/FreeInfo.h"


using namespace storage;


/**
 * Check that loading a devicegraph with Resize- and ContentInfo makes the
 * info available in the probed and staging device graph.
 */
BOOST_AUTO_TEST_CASE(load)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("test1-devicegraph.xml");

    Storage storage(environment);
    storage.probe();
    storage.check();

    {
	const Devicegraph* probed = storage.get_probed();

	const Partition* partition = Partition::find_by_name(probed, "/dev/sdb1");
	const BlkFilesystem* blk_filesystem = partition->get_blk_filesystem();

	ResizeInfo resize_info = blk_filesystem->detect_resize_info();
	BOOST_CHECK_EQUAL(resize_info.resize_ok, true);
	BOOST_CHECK_EQUAL(resize_info.min_size, 1000 * KiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 2000000 * KiB);

	ContentInfo content_info = blk_filesystem->detect_content_info();
	BOOST_CHECK_EQUAL(content_info.is_windows, true);
	BOOST_CHECK_EQUAL(content_info.is_efi, true);
	BOOST_CHECK_EQUAL(content_info.num_homes, 2);
    }

    {
	Devicegraph* staging = storage.get_staging();

	Partition* partition = Partition::find_by_name(staging, "/dev/sdb1");
	BlkFilesystem* blk_filesystem = partition->get_blk_filesystem();

	ResizeInfo resize_info = blk_filesystem->detect_resize_info();
	BOOST_CHECK_EQUAL(resize_info.resize_ok, true);
	BOOST_CHECK_EQUAL(resize_info.min_size, 1000 * KiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 2000000 * KiB);

	ContentInfo content_info = blk_filesystem->detect_content_info();
	BOOST_CHECK_EQUAL(content_info.is_windows, true);
	BOOST_CHECK_EQUAL(content_info.is_efi, true);
	BOOST_CHECK_EQUAL(content_info.num_homes, 2);
    }
}

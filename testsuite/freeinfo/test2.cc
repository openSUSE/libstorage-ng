
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/Logger.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Devices/Filesystem.h"
#include "storage/Devices/Partition.h"
#include "storage/FreeInfo.h"


using namespace storage;


/**
 * Check that modifying the Resize- and ContentInfo in the staging devicegraph
 * makes the modified values available in the probed devicegraph.
 */
BOOST_AUTO_TEST_CASE(modify)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("test2-devicegraph.xml");

    Storage storage(environment);
    storage.check();

    {
	Devicegraph* staging = storage.get_staging();
	Partition* partition = Partition::find_by_name(staging, "/dev/sdb1");

	Filesystem* filesystem = partition->get_filesystem();
	filesystem->set_resize_info(ResizeInfo(true, 1500 * KiB, 3000 * MiB));
	filesystem->set_content_info(ContentInfo(true, false, 2));

	ResizeInfo resize_info = filesystem->detect_resize_info();
	BOOST_CHECK_EQUAL(resize_info.resize_ok, true);
	BOOST_CHECK_EQUAL(resize_info.min_size, 1500 * KiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 3000 * MiB);

	ContentInfo content_info = filesystem->detect_content_info();
	BOOST_CHECK_EQUAL(content_info.is_windows, true);
	BOOST_CHECK_EQUAL(content_info.is_efi, false);
	BOOST_CHECK_EQUAL(content_info.num_homes, 2);
    }

    {
	const Devicegraph* probed = storage.get_probed();
	const Partition* partition = Partition::find_by_name(probed, "/dev/sdb1");
	const Filesystem* filesystem = partition->get_filesystem();

	const ResizeInfo& resize_info = filesystem->detect_resize_info();
	BOOST_CHECK_EQUAL(resize_info.resize_ok, true);
	BOOST_CHECK_EQUAL(resize_info.min_size, 1500 * KiB);
	BOOST_CHECK_EQUAL(resize_info.max_size, 3000 * MiB);

	ContentInfo content_info = filesystem->detect_content_info();
	BOOST_CHECK_EQUAL(content_info.is_windows, true);
	BOOST_CHECK_EQUAL(content_info.is_efi, false);
	BOOST_CHECK_EQUAL(content_info.num_homes, 2);
    }
}

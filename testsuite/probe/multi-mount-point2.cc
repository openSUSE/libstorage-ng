
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/UsedFeatures.h"
#include "storage/Devices/Partition.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"

#include "testsuite/helpers/TsCmp.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(multi_mount_point2)
{
    set_logger(get_stdout_logger());

    // Scenario: one active fstab mount point (/) and another proc mount point (/root2) for the same
    // device (sda2).
    Environment environment(true, ProbeMode::READ_MOCKUP, TargetMode::DIRECT);
    environment.set_mockup_filename("multi-mount-point-mockup2.xml");

    Storage storage(environment);
    storage.probe();

    const Devicegraph* probed = storage.get_probed();
    probed->check();

    const Partition* partition = Partition::find_by_name(probed, "/dev/sda2");

    const string mount_point = partition->get_blk_filesystem()->get_mount_point()->get_path();

    BOOST_CHECK_EQUAL(mount_point, "/");
}

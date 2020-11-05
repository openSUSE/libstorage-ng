
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/CompoundAction.h"
#include "storage/Devices/Bcache.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(flashonly_bcache_format_sentence)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::READ_DEVICEGRAPH, TargetMode::DIRECT);
    environment.set_devicegraph_filename("../probe/bcache2-devicegraph.xml");

    Storage storage(environment);
    storage.probe();

    Devicegraph* staging = storage.get_staging();

    Bcache* bcache2 = Bcache::find_by_name(staging, "/dev/bcache2");

    bcache2->remove_descendants(View::REMOVE);

    BlkFilesystem* ext4 = bcache2->create_blk_filesystem(FsType::EXT4);
    ext4->create_mount_point("/data");

    const Actiongraph* actiongraph = storage.calculate_actiongraph();
    const CompoundAction* compound_action = CompoundAction::find_by_target_device(actiongraph, bcache2);

    BOOST_REQUIRE(compound_action);

    string expected = "Format bcache /dev/bcache2 on /dev/sdb1 (100.00 MiB) for /data with ext4";

    BOOST_CHECK_EQUAL(compound_action->sentence(), expected);
}

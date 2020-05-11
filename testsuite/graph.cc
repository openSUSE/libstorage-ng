
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/BtrfsSubvolume.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Holders/Snapshot.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(create1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    BtrfsSubvolume* btrfs_subvolume1 = BtrfsSubvolume::create(devicegraph, "1");
    BtrfsSubvolume* btrfs_subvolume2 = BtrfsSubvolume::create(devicegraph, "1/2");

    // creating two holders of different type is allowed
    Subdevice::create(devicegraph, btrfs_subvolume1, btrfs_subvolume2);
    Snapshot::create(devicegraph, btrfs_subvolume1, btrfs_subvolume2);

    // but two holders of the same type is not allowed
    BOOST_CHECK_THROW(Subdevice::create(devicegraph, btrfs_subvolume1, btrfs_subvolume2),
		      HolderAlreadyExists);
}

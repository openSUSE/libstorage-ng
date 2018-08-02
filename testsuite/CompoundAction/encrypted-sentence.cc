
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/Ext4.h"
#include "storage/Devices/Encryption.h"

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;

BOOST_FIXTURE_TEST_SUITE(btrfs_subvolume_sentence, test::CompoundActionFixture)

// test for https://bugzilla.suse.com/show_bug.cgi?id=1099181
BOOST_AUTO_TEST_CASE(test_sentence_on_entrypted_not_mounted)
{
    initialize_with_devicegraph("devicegraph.xml");

    auto partition = Partition::find_by_name(staging, "/dev/sda1");
    partition->remove_descendants();

    auto encryption = partition->create_encryption("cr_sda2");
    encryption->create_blk_filesystem(FsType::EXT4);

    const auto actiongraph = storage->calculate_actiongraph();
    auto compound_action = find_compound_action_by_target(actiongraph, partition);

    BOOST_REQUIRE(compound_action);
    BOOST_CHECK_EQUAL(compound_action->sentence(), "Encrypt partition /dev/sda1 (2.01 GiB) with ext4");
}


BOOST_AUTO_TEST_SUITE_END()

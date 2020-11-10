
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/Btrfs.h"
#include "storage/Filesystems/BtrfsQgroup.h"
#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"

#include "testsuite/CompoundAction/Fixture.h"


using namespace storage;


BOOST_FIXTURE_TEST_SUITE(lvm_lv_sentence, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_create1)
{
    initialize_staging_with_one_disk();
    copy_staging_to_probed();

    sda2 = sda_gpt->create_partition("/dev/sda2", Region(5 * 2048, 500 * 2048, 512), PartitionType::PRIMARY);

    Btrfs* btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
    btrfs->set_quota(true);

    BtrfsQgroup* qgroup1 = btrfs->create_btrfs_qgroup(BtrfsQgroup::id_t(1, 0));
    qgroup1->set_referenced_limit(128 * MiB);

    const Actiongraph* actiongraph = storage->calculate_actiongraph();
    vector<const CompoundAction*> compound_actions = actiongraph->get_compound_actions();

    BOOST_REQUIRE_EQUAL(compound_actions.size(), 3);

    BOOST_CHECK_EQUAL(compound_actions[0]->sentence(), "Create partition /dev/sda2 (500.00 MiB) with btrfs");
    BOOST_CHECK_EQUAL(compound_actions[1]->sentence(), "Enable quota on /dev/sda2 (500.00 MiB)");
    BOOST_CHECK_EQUAL(compound_actions[2]->sentence(), "Modify btrfs qgroups on /dev/sda2 (500.00 MiB)");
}


BOOST_AUTO_TEST_CASE(test_create2)
{
    initialize_staging_with_three_partitions();
    copy_staging_to_probed();

    Btrfs* btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
    btrfs->add_device(sda3);
    btrfs->set_quota(true);

    btrfs->create_btrfs_qgroup(BtrfsQgroup::id_t(1, 0));
    btrfs->create_btrfs_qgroup(BtrfsQgroup::id_t(1, 1));

    const Actiongraph* actiongraph = storage->calculate_actiongraph();
    vector<const CompoundAction*> compound_actions = actiongraph->get_compound_actions();

    BOOST_REQUIRE_EQUAL(compound_actions.size(), 3);

    BOOST_CHECK_EQUAL(compound_actions[0]->sentence(),
		      "Create file system btrfs on /dev/sda2 (500.00 MiB) and /dev/sda3 (500.00 MiB)");
    BOOST_CHECK_EQUAL(compound_actions[1]->sentence(),
		      "Enable quota on /dev/sda2 (500.00 MiB) and /dev/sda3 (500.00 MiB)");
    BOOST_CHECK_EQUAL(compound_actions[2]->sentence(),
		      "Modify btrfs qgroups on /dev/sda2 (500.00 MiB) and /dev/sda3 (500.00 MiB)");
}


BOOST_AUTO_TEST_CASE(test_enable1)
{
    initialize_staging_with_three_partitions();
    Btrfs* btrfs = to_btrfs(sda2->create_blk_filesystem(FsType::BTRFS));
    copy_staging_to_probed();

    btrfs->set_quota(true);

    btrfs->create_btrfs_qgroup(BtrfsQgroup::id_t(1, 0));

    const Actiongraph* actiongraph = storage->calculate_actiongraph();
    vector<const CompoundAction*> compound_actions = actiongraph->get_compound_actions();

    BOOST_REQUIRE_EQUAL(compound_actions.size(), 2);

    BOOST_CHECK_EQUAL(compound_actions[0]->sentence(), "Enable quota on /dev/sda2 (500.00 MiB)");
    BOOST_CHECK_EQUAL(compound_actions[1]->sentence(), "Create qgroup 1/0 on /dev/sda2 (500.00 MiB)");
}


BOOST_AUTO_TEST_SUITE_END()

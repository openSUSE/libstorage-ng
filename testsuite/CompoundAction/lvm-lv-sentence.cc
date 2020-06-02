
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <memory>

#include <boost/test/unit_test.hpp>

#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/MountPoint.h"

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(lvm_lv_sentence, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_sentence_on_creating)
{
    initialize_staging_with_two_partitions();

    auto vg = LvmVg::create(staging, "vg-name");
    vg->add_lvm_pv(sda2);
    auto lv = vg->create_lvm_lv("lv-name", LvType::NORMAL, 10 * GiB);
    auto ext4 = to_ext4(lv->create_blk_filesystem(FsType::EXT4));
    ext4->create_mount_point("/test");

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, lv);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create LVM logical volume /dev/vg-name/lv-name (10.00 GiB) on volume group vg-name for /test with ext4");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_with_encryption)
{
    initialize_staging_with_two_partitions();

    auto vg = LvmVg::create(staging, "vg-name");
    vg->add_lvm_pv(sda2);
    auto lv = vg->create_lvm_lv("lv-name", LvType::NORMAL, 10 * GiB);
    Encryption* encryption = lv->create_encryption("cr_sda2", EncryptionType::LUKS1);
    auto ext4 = to_ext4(encryption->create_blk_filesystem(FsType::EXT4));
    ext4->create_mount_point("/test");

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, lv);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create encrypted LVM logical volume /dev/vg-name/lv-name (10.00 GiB) on volume group vg-name for /test with ext4");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_with_swap)
{
    initialize_staging_with_two_partitions();

    auto vg = LvmVg::create(staging, "vg-name");
    vg->add_lvm_pv(sda2);
    auto lv = vg->create_lvm_lv("lv-name", LvType::NORMAL, 2 * GiB);
    lv->create_blk_filesystem(FsType::SWAP);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, lv);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create LVM logical volume /dev/vg-name/lv-name (2.00 GiB) on volume group vg-name for swap");
}


BOOST_AUTO_TEST_SUITE_END()

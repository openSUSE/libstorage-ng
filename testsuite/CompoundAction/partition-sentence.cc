
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/Ext4.h"
#include "storage/Filesystems/MountPoint.h"

#include "testsuite/CompoundAction/Fixture.h"

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(partition_sentence, test::CompoundActionFixture)


BOOST_AUTO_TEST_CASE(test_sentence_on_creating)
{
    initialize_staging_with_two_partitions();

    auto ext4 = to_ext4(sda2->create_blk_filesystem(FsType::EXT4));
    ext4->create_mount_point("/test");

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda2);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create partition /dev/sda2 (500.00 MiB) for /test with ext4");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_with_encryption)
{
    initialize_staging_with_two_partitions();

    auto encryption = sda2->create_encryption("cr_sda2");

    auto ext4 = to_ext4(encryption->create_blk_filesystem(FsType::EXT4));
    ext4->create_mount_point("/test");

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda2);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create encrypted partition /dev/sda2 (500.00 MiB) for /test with ext4");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_as_pv)
{
    initialize_staging_with_two_partitions();

    auto lvm_vg = LvmVg::create(staging, "vg-name");
    lvm_vg->add_lvm_pv(sda2);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda2);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create partition /dev/sda2 (500.00 MiB) as LVM physical volume");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_with_encryption_as_pv)
{
    initialize_staging_with_two_partitions();

    auto encryption = sda2->create_encryption("cr_sda2");

    auto lvm_vg = LvmVg::create(staging, "vg-name");
    lvm_vg->add_lvm_pv(encryption);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda2);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create encrypted partition /dev/sda2 (500.00 MiB) as LVM physical volume");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_with_swap)
{
    initialize_staging_with_two_partitions();

    sda2->create_blk_filesystem(FsType::SWAP);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda2);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create partition /dev/sda2 (500.00 MiB) for swap");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_creating_as_bios)
{
    initialize_staging_with_two_partitions();

    sda1->set_id(ID_BIOS_BOOT);

    auto actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, sda1);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Create partition /dev/sda1 (4.00 MiB) as BIOS Boot Partition");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_mounting)
{
    initialize_with_devicegraph("devicegraph.xml");

    auto partition = Partition::find_by_name(staging, "/dev/sda3");

    auto fs = partition->get_blk_filesystem();
    fs->create_mount_point("/home");

    const Actiongraph* actiongraph = storage->calculate_actiongraph();

    auto compound_action = find_compound_action_by_target(actiongraph, partition);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Mount partition /dev/sda3 (42.27 GiB) at /home");
}


BOOST_AUTO_TEST_CASE(test_sentence_on_deleting)
{
    initialize_with_devicegraph("devicegraph.xml");

    auto partition_name = "/dev/sda2";

    delete_partition(partition_name);

    const Actiongraph* actiongraph = storage->calculate_actiongraph();

    auto deleted_partition = Partition::find_by_name(storage->get_system(), partition_name);

    auto compound_action = find_compound_action_by_target(actiongraph, deleted_partition);

    BOOST_REQUIRE(compound_action);

    BOOST_CHECK_EQUAL(compound_action->sentence(), "Delete partition /dev/sda2 (28.50 GiB)");
}


BOOST_AUTO_TEST_SUITE_END()

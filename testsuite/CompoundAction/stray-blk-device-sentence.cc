
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/StrayBlkDevice.h"
#include "storage/Devices/Encryption.h"
#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"

#include "testsuite/CompoundAction/Fixture.h"


namespace storage
{
    namespace test
    {
	struct StrayBlkDeviceFixture: public CompoundActionFixture
        {
	    void
	    initialize_probed_with_three_strays()
	    {
		Environment environment( true, ProbeMode::NONE, TargetMode::IMAGE );
		storage = make_unique<Storage>(environment);
		staging = storage->get_staging();

                // StrayBlkDevice regions always start at 0

		stray1 = StrayBlkDevice::create( staging, "/dev/vda1", Region( 0,    4 * 2048, 512 ) );
		stray2 = StrayBlkDevice::create( staging, "/dev/vda2", Region( 0,  500 * 2048, 512 ) );
		stray3 = StrayBlkDevice::create( staging, "/dev/vda3", Region( 0, 4096 * 2048, 512 ) );

                copy_staging_to_probed();
	    }

	    StrayBlkDevice* stray1 = nullptr;
	    StrayBlkDevice* stray2 = nullptr;
	    StrayBlkDevice* stray3 = nullptr;
        };
    }
}

using namespace storage;


BOOST_FIXTURE_TEST_SUITE(stray_blk_device_sentence, test::StrayBlkDeviceFixture)


BOOST_AUTO_TEST_CASE( test_format_and_mount )
{
    initialize_probed_with_three_strays();

    StrayBlkDevice * stray = StrayBlkDevice::find_by_name( staging, "/dev/vda3" );
    BlkFilesystem  * fs    = stray->create_blk_filesystem( FsType::EXT4 );
    fs->create_mount_point( "/home" );
    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray );

    BOOST_REQUIRE( compound_action) ;

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Format partition /dev/vda3 (4.00 GiB) for /home with ext4" );
}


BOOST_AUTO_TEST_CASE( test_format_no_mount )
{
    initialize_probed_with_three_strays();

    StrayBlkDevice * stray = StrayBlkDevice::find_by_name( staging, "/dev/vda3" );
    stray->create_blk_filesystem( FsType::EXT4 );
    // Not mounting this filesystem

    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray );

    BOOST_REQUIRE( compound_action) ;

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Format partition /dev/vda3 (4.00 GiB) with ext4" );
}


BOOST_AUTO_TEST_CASE( test_mount_no_format )
{
    initialize_probed_with_three_strays();

    StrayBlkDevice * stray = StrayBlkDevice::find_by_name( staging, "/dev/vda3" );
    BlkFilesystem  * fs    = stray->create_blk_filesystem( FsType::EXT4 );
    // Pretend the filesystem was already there during probing
    copy_staging_to_probed();

    fs->create_mount_point( "/home" );

    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray );

    BOOST_REQUIRE( compound_action) ;

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Mount partition /dev/vda3 (4.00 GiB) at /home");
}


BOOST_AUTO_TEST_CASE( test_swap )
{
    initialize_probed_with_three_strays();

    StrayBlkDevice * stray = StrayBlkDevice::find_by_name( staging, "/dev/vda3" );
    BlkFilesystem  * swap  = stray->create_blk_filesystem( FsType::SWAP );
    swap->create_mount_point( "swap" );
    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray );

    BOOST_REQUIRE( compound_action) ;

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Format partition /dev/vda3 (4.00 GiB) as swap" );
}


BOOST_AUTO_TEST_CASE( test_encrypted_swap )
{
    initialize_probed_with_three_strays();

    StrayBlkDevice * stray      = StrayBlkDevice::find_by_name( staging, "/dev/vda3" );
    Encryption     * encryption = stray3->create_encryption("cr_vda3", EncryptionType::LUKS1);
    BlkFilesystem  * swap       = encryption->create_blk_filesystem( FsType::SWAP );
    swap->create_mount_point( "swap" );
    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray );

    BOOST_REQUIRE( compound_action) ;

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Format partition /dev/vda3 (4.00 GiB) as encryped swap" );
}


BOOST_AUTO_TEST_CASE( test_lvm_pv )
{
    initialize_probed_with_three_strays();

    LvmVg * lvm_vg = LvmVg::create( staging, "test-vg" );
    lvm_vg->add_lvm_pv( stray3 );

    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target(actiongraph, stray3);

    BOOST_REQUIRE( compound_action );

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Create LVM physical volume over /dev/vda3 (4.00 GiB)" );
}


BOOST_AUTO_TEST_CASE( test_encrypted_pv )
{
    initialize_probed_with_three_strays();

    Encryption* encryption = stray3->create_encryption("cr_vda3", EncryptionType::LUKS1);
    LvmVg * lvm_vg = LvmVg::create( staging, "test-vg" );
    lvm_vg->add_lvm_pv( encryption );

    const Actiongraph    * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, stray3 );

    BOOST_REQUIRE( compound_action );

    BOOST_CHECK_EQUAL( compound_action->sentence(),
                       "Create LVM physical volume over encrypted /dev/vda3 (4.00 GiB)" );
}


BOOST_AUTO_TEST_SUITE_END()

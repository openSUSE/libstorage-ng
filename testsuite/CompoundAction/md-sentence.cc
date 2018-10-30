
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "storage/Devices/Md.h"
#include "storage/Devices/Encryption.h"

#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"

#include "testsuite/CompoundAction/Fixture.h"

#define BLOCK_SIZE	1024L
#define kiB             1L
#define MiB		(1024L*kiB)
#define GiB		(1024L*MiB)
#define TiB		(1024L*GiB)


using std::cout;
using std::endl;


namespace storage
{
    namespace test
    {
	struct MdFixture: public CompoundActionFixture
	{
	    MdFixture()
	    {
                create_disks();
            }


	    void create_disks()
	    {
		Environment environment(true, ProbeMode::NONE, TargetMode::IMAGE);
		storage = shared_ptr<Storage>(new Storage(environment));
		staging = storage->get_staging();

                for ( int i=0; i < 5; ++i )
                {
                    string device_name = "/dev/sd";
                    device_name += 'a' + i;

                    // cout << "Creating disk " << device_name << endl;
                    Disk * disk = Disk::create( staging, device_name, Region( 0, 512*GiB, BLOCK_SIZE ) );
                    disks.push_back( disk );
                }

		copy_staging_to_probed();
	    }


            Md * create_raid( const string & raid_name, MdLevel md_level )
            {
                Md * raid = Md::create( staging, raid_name );
                raid->set_md_level( md_level );

                for ( unsigned i=0; i < raid->minimal_number_of_devices() && i < disk_count(); ++i )
                    raid->add_device( disks[i] );

                return raid;
            }


            unsigned disk_count() const { return disks.size(); }

	    vector<Disk *> disks;
	};
    }
}

using namespace storage;

BOOST_FIXTURE_TEST_SUITE( md_raid_sentence, test::MdFixture )


BOOST_AUTO_TEST_CASE( test_create_format_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID6 );

    BlkFilesystem * fs = raid->create_blk_filesystem( FsType::EXT4 );
    fs->create_mount_point( "/data" );


    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected =
        "Create RAID6 /dev/md0 (1023.75 GiB) for /data with ext4\n"
        "from /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB), /dev/sdc (512.00 GiB), /dev/sdd (512.00 GiB)";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_create_encrypt_format_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID1 );

    Encryption * encryption = raid->create_encryption( "cr_raid0" );
    BlkFilesystem * fs = encryption->create_blk_filesystem( FsType::XFS );
    fs->create_mount_point( "/secret" );


    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected =
        "Create encrypted RAID1 /dev/md0 (511.87 GiB) for /secret with xfs\n"
        "from /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB)";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_create_encrypt_format_no_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID1 );

    Encryption * encryption = raid->create_encryption( "cr_raid0" );
    encryption->create_blk_filesystem( FsType::XFS );

    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected =
        "Create encrypted RAID1 /dev/md0 (511.87 GiB) with xfs\n"
        "from /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB)";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_just_create )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID1 );

    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected =
        "Create RAID1 /dev/md0 (511.87 GiB) from /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB)";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_format_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID6 );
    copy_staging_to_probed();

    BlkFilesystem * fs = raid->create_blk_filesystem( FsType::EXT4 );
    fs->create_mount_point( "/data" );


    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected = "Format RAID6 /dev/md0 (1023.75 GiB) for /data with ext4";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_encrypt_format_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID5 );
    copy_staging_to_probed();

    Encryption * encryption = raid->create_encryption( "cr_raid0" );
    BlkFilesystem * fs = encryption->create_blk_filesystem( FsType::XFS );

    fs->create_mount_point( "/data" );

    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected = "Encrypt RAID5 /dev/md0 (1023.75 GiB) for /data with xfs";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_just_mount )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID5 );
    BlkFilesystem * fs = raid->create_blk_filesystem( FsType::EXT4 );
    copy_staging_to_probed();

    fs->create_mount_point( "/data" );


    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected = "Mount RAID5 /dev/md0 (1023.75 GiB) at /data";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_create_encrypted_swap )
{
    Md * raid = create_raid( "/dev/md1", MdLevel::RAID1 );

    Encryption * encryption = raid->create_encryption( "cr_raid1" );
    BlkFilesystem * fs = encryption->create_blk_filesystem( FsType::SWAP );
    fs->create_mount_point( "swap" );

    const Actiongraph	 * actiongraph     = storage->calculate_actiongraph();
    const CompoundAction * compound_action = find_compound_action_by_target( actiongraph, raid );

    BOOST_REQUIRE( compound_action ) ;

    string expected =
        "Create encrypted RAID1 /dev/md1 (511.87 GiB) for swap\n"
        "from /dev/sda (512.00 GiB), /dev/sdb (512.00 GiB)"
        "\nare you serious?!";

    BOOST_CHECK_EQUAL( compound_action->sentence(), expected );

#if 0
    cout << "\nEXPECTED:\n\n" << expected << "\n" << endl;
    cout << "\nACTUAL:\n\n"   << compound_action->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_SUITE_END()

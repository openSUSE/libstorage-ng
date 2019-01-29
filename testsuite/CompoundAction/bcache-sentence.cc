
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <iostream>

#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Devices/Encryption.h"

#include "storage/Filesystems/BlkFilesystem.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmPv.h"
#include "storage/Utils/HumanString.h"

#include "testsuite/CompoundAction/Fixture.h"

#define BLOCK_SIZE	512L

using std::cout;
using std::endl;


namespace storage
{
    namespace test
    {
	struct BCacheFixture: public CompoundActionFixture
	{
	    BCacheFixture():
		disk0(0),
		disk1(0),
		ssd0(0),
		ssd1(0),
                bcache0(0),
                bcache1(0),
                cset0(0),
                cset1(0)
	    {
                init_disks();
            }


	    void init_disks()
	    {
		Environment environment(true, ProbeMode::NONE, TargetMode::IMAGE);
		storage = shared_ptr<Storage>(new Storage(environment));
		staging = storage->get_staging();

		disk0 = Disk::create( staging, "/dev/sda", Region( 0,   2*TiB / BLOCK_SIZE, BLOCK_SIZE ) );
		disk1 = Disk::create( staging, "/dev/sdb", Region( 0, 512*GiB / BLOCK_SIZE, BLOCK_SIZE ) );

		ssd0  = Disk::create( staging, "/dev/sdf", Region( 0, 128*MiB / BLOCK_SIZE, BLOCK_SIZE ) );
		ssd1  = Disk::create( staging, "/dev/sdg", Region( 0, 160*MiB / BLOCK_SIZE, BLOCK_SIZE ) );

		copy_staging_to_probed();
	    }


            void init_bcaches()
            {
                init_bcache0();
                init_bcache1();
            }


            void init_bcache0(bool cset = true)
            {
                cset0 = ssd0->create_bcache_cset();
                bcache0 = disk0->create_bcache( "/dev/bcache0" );

		if(cset)
		    bcache0->attach_bcache_cset( cset0 );
            }


            void init_bcache1(bool cset = true)
            {
                cset1 = ssd1->create_bcache_cset();
                bcache1 = disk1->create_bcache( "/dev/bcache1" );

		if(cset)
		    bcache1->attach_bcache_cset( cset1 );
            }


	    Disk* 	disk0;
	    Disk* 	disk1;
	    Disk* 	ssd0;
	    Disk* 	ssd1;
	    Bcache* 	bcache0;
	    Bcache* 	bcache1;
	    BcacheCset* cset0;
	    BcacheCset* cset1;
	};
    }
}

using namespace storage;

BOOST_FIXTURE_TEST_SUITE( bcache_sentence, test::BCacheFixture )


BOOST_AUTO_TEST_CASE( test_create )
{
    init_bcaches();

    BlkFilesystem * ext4 = bcache0->create_blk_filesystem( FsType::EXT4 );
    ext4->create_mount_point( "/data" );

    Encryption * encryption = bcache1->create_encryption( "cr_bcache1" );
    BlkFilesystem * xfs = encryption->create_blk_filesystem( FsType::XFS );
    xfs->create_mount_point( "/home" );

    const Actiongraph	 * actiongraph = storage->calculate_actiongraph();
    const CompoundAction * compound_action0 = find_compound_action_by_target( actiongraph, bcache0 );
    const CompoundAction * compound_action1 = find_compound_action_by_target( actiongraph, bcache1 );

    BOOST_REQUIRE( compound_action0 ) ;
    BOOST_REQUIRE( compound_action1 ) ;

    string expected0 =
        "Create bcache /dev/bcache0 on /dev/sda (2.00 TiB) for /data with ext4\n"
        "/dev/bcache0 is cached by /dev/sdf (128.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action0->sentence(), expected0 );


    string expected1 =
        "Create encrypted bcache /dev/bcache1 on /dev/sdb (512.00 GiB) for /home with xfs\n"
        "/dev/bcache1 is cached by /dev/sdg (160.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action1->sentence(), expected1 );

#if 0
    cout << "\nexpected0:\n" << expected0 << "\n" << endl;
    cout << "\naction0:\n"   << compound_action0->sentence() << "\n" << endl;

    cout << "\nexpected1:\n" << expected1 << "\n" << endl;
    cout << "\naction1:\n"   << compound_action1->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_format )
{
    init_bcaches();
    copy_staging_to_probed();

    BlkFilesystem * ext4 = bcache0->create_blk_filesystem( FsType::EXT4 );
    ext4->create_mount_point( "/data" );

    Encryption * encryption = bcache1->create_encryption( "cr_bcache1" );
    BlkFilesystem * xfs = encryption->create_blk_filesystem( FsType::XFS );
    xfs->create_mount_point( "/home" );

    const Actiongraph	 * actiongraph = storage->calculate_actiongraph();
    const CompoundAction * compound_action0 = find_compound_action_by_target( actiongraph, bcache0 );
    const CompoundAction * compound_action1 = find_compound_action_by_target( actiongraph, bcache1 );

    BOOST_REQUIRE( compound_action0 ) ;
    BOOST_REQUIRE( compound_action1 ) ;

    string expected0 =
        "Format bcache /dev/bcache0 on /dev/sda (2.00 TiB) for /data with ext4\n"
        "/dev/bcache0 is cached by /dev/sdf (128.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action0->sentence(), expected0 );


    string expected1 =
        "Encrypt bcache /dev/bcache1 on /dev/sdb (512.00 GiB) for /home with xfs\n"
        "/dev/bcache1 is cached by /dev/sdg (160.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action1->sentence(), expected1 );

#if 0
    cout << "\nexpected0:\n" << expected0 << "\n" << endl;
    cout << "\naction0:\n"   << compound_action0->sentence() << "\n" << endl;

    cout << "\nexpected1:\n" << expected1 << "\n" << endl;
    cout << "\naction1:\n"   << compound_action1->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_CASE( test_mount )
{
    init_bcaches();

    BlkFilesystem * ext4 = bcache0->create_blk_filesystem( FsType::EXT4 );

    Encryption * encryption = bcache1->create_encryption( "cr_bcache1" );
    BlkFilesystem * xfs = encryption->create_blk_filesystem( FsType::XFS );
    
    copy_staging_to_probed();
    
    ext4->create_mount_point( "/data" );
    xfs->create_mount_point( "/home" );

    const Actiongraph	 * actiongraph = storage->calculate_actiongraph();
    const CompoundAction * compound_action0 = find_compound_action_by_target( actiongraph, bcache0 );
    const CompoundAction * compound_action1 = find_compound_action_by_target( actiongraph, bcache1 );

    BOOST_REQUIRE( compound_action0 ) ;
    BOOST_REQUIRE( compound_action1 ) ;

    string expected0 =
        "Mount bcache /dev/bcache0 on /dev/sda (2.00 TiB) at /data\n"
        "/dev/bcache0 is cached by /dev/sdf (128.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action0->sentence(), expected0 );


    string expected1 =
        "Mount bcache /dev/bcache1 on /dev/sdb (512.00 GiB) at /home\n"
        "/dev/bcache1 is cached by /dev/sdg (160.00 MiB)";

    BOOST_CHECK_EQUAL( compound_action1->sentence(), expected1 );

#if 0
    cout << "\nexpected0:\n" << expected0 << "\n" << endl;
    cout << "\naction0:\n"   << compound_action0->sentence() << "\n" << endl;

    cout << "\nexpected1:\n" << expected1 << "\n" << endl;
    cout << "\naction1:\n"   << compound_action1->sentence() << "\n" << endl;
#endif
}


BOOST_AUTO_TEST_SUITE_END()

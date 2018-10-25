
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

#include "testsuite/CompoundAction/Fixture.h"

#define BLOCK_SIZE	1024L
#define kiB             1L
#define MiB		(1024L*kiB)
#define GiB		(1024L*MiB)
#define TiB		(1024L*GiB)


// DEBUG
// DEBUG
// DEBUG
#include <iostream>
using std::cout;
using std::endl;
// DEBUG
// DEBUG
// DEBUG


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
		ssd2(0)
	    {
                init_disks();
            }


	    void init_disks()
	    {
		Environment environment(true, ProbeMode::NONE, TargetMode::IMAGE);
		storage = shared_ptr<Storage>(new Storage(environment));
		staging = storage->get_staging();

		disk0 = Disk::create( staging, "/dev/sda", Region( 0,   2*TiB, BLOCK_SIZE ) );
		disk1 = Disk::create( staging, "/dev/sdb", Region( 0, 512*MiB, BLOCK_SIZE ) );

		ssd0  = Disk::create( staging, "/dev/sdf", Region( 0, 128*MiB, BLOCK_SIZE ) );
		ssd1  = Disk::create( staging, "/dev/sdg", Region( 0, 160*MiB, BLOCK_SIZE ) );
		ssd2  = Disk::create( staging, "/dev/sdh", Region( 0,  64*MiB, BLOCK_SIZE ) );

		copy_staging_to_probed();
	    }


	    Disk * disk0;
	    Disk * disk1;
	    Disk * ssd0;
	    Disk * ssd1;
	    Disk * ssd2;
	};
    }
}

using namespace storage;

BOOST_FIXTURE_TEST_SUITE( bcache_sentence, test::BCacheFixture )


BOOST_AUTO_TEST_CASE( test1 )
{
    BcacheCset * cset0 = ssd0->create_bcache_cset();
    Bcache * bcache0 = disk0->create_bcache( "/dev/bcache0" );
    bcache0->attach_bcache_cset( cset0 );

    BlkFilesystem * ext4 = bcache0->create_blk_filesystem( FsType::EXT4 );
    ext4->create_mount_point( "/data" );

    
    BcacheCset * cset1 = ssd1->create_bcache_cset();
    BcacheCset * cset2 = ssd2->create_bcache_cset();

    Bcache * bcache1 = disk1->create_bcache( "/dev/bcache1" );
    bcache1->attach_bcache_cset( cset1 );
    bcache1->attach_bcache_cset( cset2 );

    Encryption * encryption = bcache1->create_encryption( "cr_bcache1" );
    BlkFilesystem * xfs = encryption->create_blk_filesystem( FsType::XFS );
    xfs->create_mount_point( "/home" );


    const Actiongraph	 * actiongraph = storage->calculate_actiongraph();
    const CompoundAction * compound_action0 = find_compound_action_by_target( actiongraph, bcache0 );
    const CompoundAction * compound_action1 = find_compound_action_by_target( actiongraph, bcache1 );
    
    BOOST_REQUIRE( compound_action0 ) ;
    BOOST_REQUIRE( compound_action1 ) ;

    BOOST_CHECK_EQUAL( compound_action0->sentence(),
		       "Create Bcache /dev/bcache0 on /dev/sda (2.00 TiB) from /dev/sdf (128.00 GiB) for /data with ext4" );
    
    BOOST_CHECK_EQUAL( compound_action1->sentence(),
		       "Create encrypted Bcache /dev/bcache1 on /dev/sdb (512 GiB) from /dev/sdg (160.00 GiB), /dev/sdh (64 MiB) for /home with XFS" );
    
}


BOOST_AUTO_TEST_SUITE_END()

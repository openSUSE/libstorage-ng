
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


BOOST_AUTO_TEST_CASE( test_create )
{
    Md * raid = create_raid( "/dev/md0", MdLevel::RAID6 );
        
    // Encryption * encryption = raid->create_encryption( "cr_raid0" );
    BlkFilesystem * ext4 = raid->create_blk_filesystem( FsType::EXT4 );
    ext4->create_mount_point( "/data" );


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


BOOST_AUTO_TEST_SUITE_END()

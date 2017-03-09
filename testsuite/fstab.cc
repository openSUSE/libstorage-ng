
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <iostream>

// Enable testing of private and protected methods, too
// #define protected public
// #define private   public
#include "storage/EtcFstab.h"
#include "storage/Filesystems/FilesystemImpl.h"

using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE( parse_and_format )
{
    string_vec input = {
        /** 00 **/ "LABEL=swap        swap         swap   defaults     0  0",
        /** 01 **/ "LABEL=xfs-root    /            xfs    defaults     0  0",
        /** 02 **/ "LABEL=btrfs-root  /btrfs-root  btrfs  ro           1  2",
        /** 03 **/ "LABEL=space       /space       xfs    noauto,user  1  2"
    };

    EtcFstab fstab;
    fstab.parse( input );

    string_vec output = fstab.format_lines();

    BOOST_CHECK_EQUAL( fstab.get_entry_count(), input.size() );

    for ( int i=0; i < fstab.get_entry_count(); ++i )
        BOOST_CHECK_EQUAL( output[i], input[i] );

    int i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_device(), "LABEL=swap"       );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_device(), "LABEL=xfs-root"   );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_device(), "LABEL=btrfs-root" );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_device(), "LABEL=space"      );

    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "swap"        );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/"           );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/btrfs-root" );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space"      );

    i=0;
    BOOST_CHECK_EQUAL( toString( fstab.get_entry( i++ )->get_fs_type() ), "swap"  );
    BOOST_CHECK_EQUAL( toString( fstab.get_entry( i++ )->get_fs_type() ), "xfs"   );
    BOOST_CHECK_EQUAL( toString( fstab.get_entry( i++ )->get_fs_type() ), "btrfs" );
    BOOST_CHECK_EQUAL( toString( fstab.get_entry( i++ )->get_fs_type() ), "xfs"   );

    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_opts().empty(), true  );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_opts().empty(), true  );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_opts().empty(), false );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_opts().empty(), false );

    const MountOpts & opts_ref = fstab.get_entry( 1 )->get_mount_opts(); // xfs-root
    BOOST_CHECK_EQUAL( opts_ref.contains( "defaults" ), false );

    MountOpts opts = fstab.get_entry( 2 )->get_mount_opts(); // btrfs-root
    BOOST_CHECK_EQUAL( opts.size(), 1 );
    BOOST_CHECK_EQUAL( opts.get_opt( 0 ), "ro" );
    BOOST_CHECK_EQUAL( opts.contains( "ro" ), true );
    BOOST_CHECK_EQUAL( opts.contains( "wrglbrmpf" ), false );
    BOOST_CHECK_EQUAL( opts.contains( "defaults"  ), false );

    opts = fstab.get_entry( 3 )->get_mount_opts(); // space
    BOOST_CHECK_EQUAL( opts.size(), 2 );
    BOOST_CHECK_EQUAL( opts.contains( "user"   ), true );
    BOOST_CHECK_EQUAL( opts.contains( "noauto" ), true  );
    BOOST_CHECK_EQUAL( opts.contains( "auto"   ), false );

    opts << "umask=007" << "gid=42";
    fstab.get_entry( 3 )->set_mount_opts( opts );
    opts.clear();
    opts = fstab.get_entry( 3 )->get_mount_opts();

    BOOST_CHECK_EQUAL( opts.size(), 4 );
    BOOST_CHECK_EQUAL( opts.contains( "umask=007" ), true  );
    BOOST_CHECK_EQUAL( opts.contains( "umask"     ), false );
    BOOST_CHECK_EQUAL( opts.contains( "gid=42"    ), true  );

    BOOST_CHECK_EQUAL( opts.format(), "noauto,user,umask=007,gid=42" );


    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_dump_pass(), 0 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_dump_pass(), 0 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_dump_pass(), 1 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_dump_pass(), 1 );

    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_fsck_pass(), 0 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_fsck_pass(), 0 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_fsck_pass(), 2 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_fsck_pass(), 2 );
}


BOOST_AUTO_TEST_CASE( mount_order )
{
    // Wrong mount order by intention
    string_vec input = {
        /** 00 **/ "LABEL=var-log  /var/log     ext4   defaults     1  2",
        /** 01 **/ "LABEL=var      /var         ext4   defaults     1  2",
        /** 02 **/ "LABEL=walk     /space/walk  xfs    noauto,user  1  2",
        /** 03 **/ "LABEL=space    /space       xfs    noauto,user  1  2",
        /** 04 **/ "LABEL=root     /            ext4   defaults     1  1"
    };

    EtcFstab fstab;
    fstab.parse( input );

    //
    // Check and fix mount order
    //

    BOOST_CHECK_EQUAL( fstab.check_mount_order(), false );

    fstab.fix_mount_order();

    BOOST_CHECK_EQUAL( fstab.check_mount_order(), true );

    int i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry_count(), 5 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/"           );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/var"        );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/var/log"    );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space"      );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space/walk" );


    //
    // Take out /var and re-insert it
    //

    FstabEntry * var_entry = fstab.find_mount_point( "/var" );
    BOOST_CHECK_EQUAL( var_entry != 0, true );
    BOOST_CHECK_EQUAL( var_entry->get_device(), "LABEL=var" );

    int index = fstab.get_index_of( var_entry );
    BOOST_CHECK_EQUAL( index, 1 );
    CommentedConfigFile::Entry * entry = fstab.take( index );
    BOOST_CHECK_EQUAL( entry == var_entry, true );

    BOOST_CHECK_EQUAL( fstab.get_entry( 1 )->get_mount_point(), "/var/log" );
    fstab.add( var_entry );
    BOOST_CHECK_EQUAL( fstab.get_entry( 1 )->get_mount_point(), "/var"     );
    BOOST_CHECK_EQUAL( fstab.get_entry( 2 )->get_mount_point(), "/var/log" );


    //
    // Take out /var and /var/log and reinsert them (they go to the end now)
    //

    var_entry                  = fstab.find_mount_point( "/var" );
    FstabEntry * var_log_entry = fstab.find_mount_point( "/var/log" );

    fstab.take( fstab.get_index_of( var_log_entry ) );
    fstab.take( fstab.get_index_of( var_entry     ) );

    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry_count(), 3 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/"           );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space"      );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space/walk" );

    fstab.add( var_entry     );
    fstab.add( var_log_entry );

    i=0;
    BOOST_CHECK_EQUAL( fstab.get_entry_count(), 5 );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/"           );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space"      );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/space/walk" );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/var"        );
    BOOST_CHECK_EQUAL( fstab.get_entry( i++ )->get_mount_point(), "/var/log"    );
}

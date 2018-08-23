
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Filesystems/MountPoint.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(normalize_path)
{
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "" ), "" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "swap" ), "swap" );

    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "/" ), "/" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "/foo" ), "/foo" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "/foo/bar" ), "/foo/bar" );

    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "///foo" ), "/foo" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "///foo/" ), "/foo" );

    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "//foo/bar" ), "/foo/bar" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "///foo///bar" ), "/foo/bar" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "///foo///bar//" ), "/foo/bar" );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "///foo///bar//" ), "/foo/bar" );

    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "////foo///bar/." ), "/foo/bar/." );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "////foo///bar/.." ), "/foo/bar/.." );
    BOOST_CHECK_EQUAL( MountPoint::normalize_path( "////foo//..///./bar//" ), "/foo/.././bar" );
}

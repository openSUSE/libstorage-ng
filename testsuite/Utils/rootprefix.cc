
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/MountPointPath.h"


using namespace std::literals;
using namespace storage;


BOOST_AUTO_TEST_CASE(ctor1)
{
    MountPointPath mount_point_path("/test", ""s);

    BOOST_CHECK_EQUAL(mount_point_path.path, "/test");
    BOOST_CHECK_EQUAL(mount_point_path.rootprefixed, true);
}


BOOST_AUTO_TEST_CASE(ctor2)
{
    MountPointPath mount_point_path("/test", "/"s);

    BOOST_CHECK_EQUAL(mount_point_path.path, "/test");
    BOOST_CHECK_EQUAL(mount_point_path.rootprefixed, true);
}


BOOST_AUTO_TEST_CASE(ctor3)
{
    MountPointPath mount_point_path("/test", "/mnt"s);

    BOOST_CHECK_EQUAL(mount_point_path.path, "/test");
    BOOST_CHECK_EQUAL(mount_point_path.rootprefixed, false);
}


BOOST_AUTO_TEST_CASE(ctor4)
{
    MountPointPath mount_point_path("/mnt/test", "/mnt"s);

    BOOST_CHECK_EQUAL(mount_point_path.path, "/test");
    BOOST_CHECK_EQUAL(mount_point_path.rootprefixed, true);
}


BOOST_AUTO_TEST_CASE(fullpath1)
{
    MountPointPath mount_point_path("/", false);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "/");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "/");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "/");
}


BOOST_AUTO_TEST_CASE(fullpath2)
{
    MountPointPath mount_point_path("/", true);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "/");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "/");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "/mnt");
}


BOOST_AUTO_TEST_CASE(fullpath3)
{
    MountPointPath mount_point_path("/test", false);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "/test");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "/test");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "/test");
}


BOOST_AUTO_TEST_CASE(fullpath4)
{
    MountPointPath mount_point_path("/test", true);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "/test");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "/test");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "/mnt/test");
}


BOOST_AUTO_TEST_CASE(swap1)
{
    MountPointPath mount_point_path("swap", false);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "swap");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "swap");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "swap");
}


BOOST_AUTO_TEST_CASE(swap2)
{
    MountPointPath mount_point_path("swap", true);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "swap");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "swap");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "swap");
}


BOOST_AUTO_TEST_CASE(swap3)
{
    MountPointPath mount_point_path("none", false);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "none");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "none");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "none");
}


BOOST_AUTO_TEST_CASE(swap4)
{
    MountPointPath mount_point_path("none", true);

    BOOST_CHECK_EQUAL(mount_point_path.fullpath(""), "none");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/"), "none");
    BOOST_CHECK_EQUAL(mount_point_path.fullpath("/mnt"), "none");
}

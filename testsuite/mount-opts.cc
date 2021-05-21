
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/EtcFstab.h"
#include "storage/Filesystems/FilesystemImpl.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(no_subvol)
{
    MountOpts mount_opts;

    BOOST_CHECK(!mount_opts.has_subvol());

    BOOST_CHECK(!mount_opts.has_subvol(262, "home"));
}


BOOST_AUTO_TEST_CASE(subvol_id)
{
    MountOpts mount_opts("subvolid=262");

    BOOST_CHECK(mount_opts.has_subvol());

    BOOST_CHECK(mount_opts.has_subvol(262, "home"));
    BOOST_CHECK(!mount_opts.has_subvol(999, "home"));
}


BOOST_AUTO_TEST_CASE(subvol_id_leading_zero)
{
    MountOpts mount_opts("subvolid=0262");

    BOOST_CHECK(mount_opts.has_subvol());

    BOOST_CHECK(mount_opts.has_subvol(262, "home"));
    BOOST_CHECK(!mount_opts.has_subvol(999, "home"));
}


BOOST_AUTO_TEST_CASE(subvol_path)
{
    MountOpts mount_opts("subvol=home");

    BOOST_CHECK(mount_opts.has_subvol());

    BOOST_CHECK(mount_opts.has_subvol(262, "home"));
    BOOST_CHECK(!mount_opts.has_subvol(262, "does-not-exist"));
}


BOOST_AUTO_TEST_CASE(subvol_path_leading_slash)
{
    MountOpts mount_opts("subvol=/home");

    BOOST_CHECK(mount_opts.has_subvol());

    BOOST_CHECK(mount_opts.has_subvol(262, "home"));
    BOOST_CHECK(!mount_opts.has_subvol(262, "does-not-exist"));
}


BOOST_AUTO_TEST_CASE(test_ctor)
{
    BOOST_CHECK(MountOpts("").empty());
    BOOST_CHECK(MountOpts(vector<string>({})).empty());

    BOOST_CHECK(MountOpts("defaults").empty());
    BOOST_CHECK(MountOpts(vector<string>({ "defaults" })).empty());

    BOOST_CHECK_EQUAL(boost::join(MountOpts("noauto,user").get_opts(), ","), "noauto,user");
    BOOST_CHECK_EQUAL(boost::join(MountOpts(vector<string>({ "noauto", "user" })).get_opts(), ","), "noauto,user");
}


BOOST_AUTO_TEST_CASE(test_remove)
{
    MountOpts mount_opts("ro,noauto,ro");

    mount_opts.remove("ro");
    BOOST_CHECK_EQUAL(boost::join(mount_opts.get_opts(), ","), "noauto");

    mount_opts.remove("noauto");
    BOOST_CHECK_EQUAL(boost::join(mount_opts.get_opts(), ","), "");
}


BOOST_AUTO_TEST_CASE(test_append)
{
    MountOpts mount_opts("ro");

    mount_opts.append("noauto");
    BOOST_CHECK_EQUAL(boost::join(mount_opts.get_opts(), ","), "ro,noauto");

    mount_opts << "ro";
    BOOST_CHECK_EQUAL(boost::join(mount_opts.get_opts(), ","), "ro,noauto,ro");
}

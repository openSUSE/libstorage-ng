
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>
#include <iostream>

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

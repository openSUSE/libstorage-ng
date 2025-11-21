
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/UsedFeatures.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_CHECK_EQUAL(get_used_features_names(0), "");

    BOOST_CHECK_EQUAL(get_used_features_names(UF_EXT2), "ext2");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_EXT4), "ext4");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_BTRFS), "btrfs");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_XFS), "xfs");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_BCACHEFS), "bcachefs");

    BOOST_CHECK_EQUAL(get_used_features_names(UF_EXT4 | UF_XFS), "ext4 xfs");

    BOOST_CHECK_EQUAL(get_used_features_names(UF_LVM | UF_MDRAID | UF_LUKS), "luks lvm md-raid");

    BOOST_CHECK_EQUAL(get_used_features_names(UF_DASD), "dasd");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_NVME), "nvme");
    BOOST_CHECK_EQUAL(get_used_features_names(UF_FCOE), "fcoe");
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/StorageDefines.h"
#include "storage/SystemInfo/CmdBtrfs.h"
#include "storage/SystemInfo/CmdLsscsi.h"
#include "storage/SystemInfo/CmdParted.h"


using namespace std;
using namespace storage;


// Ensure versions of external programs can be parsed. Note that usually most external
// programs are not available during package build.


BOOST_AUTO_TEST_CASE(btrfs_version)
{
    if (access(BTRFS_BIN, X_OK) != 0)
	return;

    BOOST_CHECK_NO_THROW(CmdBtrfsVersion::query_version());
}


BOOST_AUTO_TEST_CASE(lsscsi_version)
{
    if (access(LSSCSI_BIN, X_OK) != 0)
	return;

    BOOST_CHECK_NO_THROW(CmdLsscsiVersion::query_version());
}


BOOST_AUTO_TEST_CASE(parted_version)
{
    if (access(PARTED_BIN, X_OK) != 0)
	return;

    BOOST_CHECK_NO_THROW(PartedVersion::query_version());
}

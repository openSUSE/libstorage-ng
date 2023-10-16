
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Partition.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_CHECK(!is_linux_partition_id(ID_ESP));
    BOOST_CHECK(!is_linux_partition_id(ID_ESP, LinuxPartitionIdCategory::ROOT));
    BOOST_CHECK(!is_linux_partition_id(ID_ESP, LinuxPartitionIdCategory::USR));

    BOOST_CHECK(is_linux_partition_id(ID_LINUX_ROOT_X86_64));
    BOOST_CHECK(is_linux_partition_id(ID_LINUX_ROOT_X86_64, LinuxPartitionIdCategory::ROOT));
    BOOST_CHECK(!is_linux_partition_id(ID_LINUX_ROOT_X86_64, LinuxPartitionIdCategory::USR));

    BOOST_CHECK(is_linux_partition_id(ID_LINUX_USR_X86_64));
    BOOST_CHECK(is_linux_partition_id(ID_LINUX_USR_X86_64, LinuxPartitionIdCategory::USR));
    BOOST_CHECK(!is_linux_partition_id(ID_LINUX_USR_X86_64, LinuxPartitionIdCategory::ROOT));
}


BOOST_AUTO_TEST_CASE(test2)
{
    // Check that the architecture during 'make check' is known.

    SystemInfo system_info;

    BOOST_CHECK(is_linux_partition_id(get_linux_partition_id(LinuxPartitionIdCategory::ROOT, system_info),
				      LinuxPartitionIdCategory::ROOT));
}


BOOST_AUTO_TEST_CASE(test3)
{
    Mockup::set_mode(Mockup::Mode::PLAYBACK);
    Mockup::set_command(UNAME_BIN " -m", RemoteCommand({ "i686" }, {}, 0));
    Mockup::set_command(TEST_BIN " -d /sys/firmware/efi/efivars", RemoteCommand({}, {}, 0));
    Mockup::set_command(GETCONF_BIN " PAGESIZE", RemoteCommand({ "4096" }, {}, 0));

    SystemInfo system_info;

    BOOST_CHECK_EQUAL(get_linux_partition_id(LinuxPartitionIdCategory::ROOT, system_info),
		      ID_LINUX_ROOT_X86);
}

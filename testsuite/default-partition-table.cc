
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/Environment.h"
#include "storage/SystemInfo/Arch.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, PtType pt_type)
    {
	return s << toString(pt_type);
    }
}


BOOST_AUTO_TEST_CASE(test1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", 320 * GiB);

    BOOST_CHECK_EQUAL(sda->get_default_partition_table_type(), PtType::MSDOS);

    BOOST_CHECK_EQUAL(sda->get_possible_partition_table_types(),
		      vector<PtType>({ PtType::MSDOS, PtType::GPT }));
}


BOOST_AUTO_TEST_CASE(test2)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", 3 * TiB);

    BOOST_CHECK_EQUAL(sda->get_default_partition_table_type(), PtType::GPT);

    BOOST_CHECK_EQUAL(sda->get_possible_partition_table_types(), vector<PtType>({ PtType::GPT }));
}


BOOST_AUTO_TEST_CASE(test3)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);
    storage.get_impl().get_arch().set_efiboot(true);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", 320 * GiB);

    BOOST_CHECK_EQUAL(sda->get_default_partition_table_type(), PtType::GPT);

    BOOST_CHECK_EQUAL(sda->get_possible_partition_table_types(),
		      vector<PtType>({ PtType::GPT, PtType::MSDOS }));
}


BOOST_AUTO_TEST_CASE(test4)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", 4 * GiB);

    BOOST_CHECK_EQUAL(dasda->get_default_partition_table_type(), PtType::DASD);

    BOOST_CHECK_EQUAL(dasda->get_possible_partition_table_types(),
		      vector<PtType>({ PtType::DASD }));
}

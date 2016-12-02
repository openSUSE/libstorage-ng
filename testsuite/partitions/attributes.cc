
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_attributes_on_msdos)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 100000, 512));

    PartitionTable* msdos = sda->create_partition_table(PtType::MSDOS);

    Partition* sda1 = msdos->create_partition("/dev/sda1", Region(1, 2000, 512), PartitionType::PRIMARY);

    sda1->set_type(PartitionType::PRIMARY);

    sda1->set_id(ID_ESP);
    BOOST_CHECK_EQUAL(sda1->get_id(), ID_ESP);

    BOOST_CHECK_THROW(sda1->set_id(ID_BIOS_BOOT), Exception);

    sda1->set_boot(true);
    BOOST_CHECK(sda1->is_boot());

    BOOST_CHECK_THROW(sda1->set_legacy_boot(true), Exception);
}


BOOST_AUTO_TEST_CASE(test_attributes_on_gpt)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda", Region(0, 100000, 512));

    PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(1, 0, 512), PartitionType::PRIMARY);

    sda1->set_type(PartitionType::PRIMARY);

    BOOST_CHECK_THROW(sda1->set_type(PartitionType::EXTENDED), Exception);

    sda1->set_id(ID_ESP);
    BOOST_CHECK_EQUAL(sda1->get_id(), ID_ESP);

    BOOST_CHECK_THROW(sda1->set_id(ID_NTFS), Exception);

    BOOST_CHECK_THROW(sda1->set_boot(true), Exception);

    sda1->set_legacy_boot(true);
    BOOST_CHECK(sda1->is_legacy_boot());
}

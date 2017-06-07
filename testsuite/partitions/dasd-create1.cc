
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DasdImpl.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_delete_logical)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasda = Dasd::create(devicegraph, "/dev/dasda", Region(0, 601020, 4 * KiB));

    PartitionTable* dasd_pt = dasda->create_partition_table(PtType::DASD);

    dasd_pt->create_partition("/dev/dasda1", Region(131328, 130560, 4 * KiB), PartitionType::PRIMARY);

    dasd_pt->create_partition("/dev/dasda1", Region(768, 130560, 4 * KiB), PartitionType::PRIMARY);

    storage.check();
    
    vector<Partition*> partitions = dasd_pt->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 2);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/dasda1");
    BOOST_CHECK_EQUAL(partitions[0]->get_region(), Region(768, 130560, 4 * KiB));

    // originally first dasda1
    BOOST_CHECK_EQUAL(partitions[1]->get_name(), "/dev/dasda2");
    BOOST_CHECK_EQUAL(partitions[1]->get_region(), Region(131328, 130560, 4 * KiB));
}

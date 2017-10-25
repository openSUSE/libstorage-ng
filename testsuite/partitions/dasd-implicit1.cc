
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Dasd.h"
#include "storage/Devices/ImplicitPt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Utils/Region.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(test_create_implicit_eckd_ldl)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasd = Dasd::create(devicegraph, "/dev/dasda", Region(0, 601020, 4096));
    dasd->set_type(DasdType::ECKD);
    dasd->set_format(DasdFormat::LDL);

    ImplicitPt* implicit_pt = to_implicit_pt(dasd->create_partition_table(PtType::IMPLICIT));

    storage.check();

    vector<Partition*> partitions = implicit_pt->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 1);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/dasda1");
    BOOST_CHECK_EQUAL(partitions[0]->get_region(), Region(3, 601017, 4096));
}


BOOST_AUTO_TEST_CASE(test_create_implicit_fba)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Dasd* dasd = Dasd::create(devicegraph, "/dev/dasdb", Region(0, 20000, 512));
    dasd->set_type(DasdType::FBA);

    ImplicitPt* implicit_pt = to_implicit_pt(dasd->create_partition_table(PtType::IMPLICIT));

    storage.check();

    vector<Partition*> partitions = implicit_pt->get_partitions();

    BOOST_REQUIRE_EQUAL(partitions.size(), 1);

    BOOST_CHECK_EQUAL(partitions[0]->get_name(), "/dev/dasdb1");
    BOOST_CHECK_EQUAL(partitions[0]->get_region(), Region(2, 19998, 512));
}

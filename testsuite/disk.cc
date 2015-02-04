
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(disk1)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_NORMAL);

    Storage storage(environment);

    const Devicegraph* lhs = storage.get_probed();

    Devicegraph* rhs = storage.get_staging();

    Disk* sda = Disk::create(rhs, "/dev/sda");
    sda->set_size_k(16 * 1024 * 1024);

    BOOST_CHECK_EXCEPTION(Actiongraph actiongraph(storage, lhs, rhs), std::runtime_error,
	[](const std::runtime_error& e) { return e.what() == string("cannot create disk"); }
    );
}


BOOST_AUTO_TEST_CASE(disk2)
{
    storage::Environment environment(true, ProbeMode::PROBE_NONE, TargetMode::TARGET_IMAGE);

    Storage storage(environment);

    const Devicegraph* lhs = storage.get_probed();

    Devicegraph* rhs = storage.get_staging();

    Disk* sda = Disk::create(rhs, "/dev/sda");
    sda->set_size_k(16 * 1024 * 1024);

    Actiongraph actiongraph(storage, lhs, rhs);

    BOOST_CHECK_EQUAL(actiongraph.get_commit_steps().size(), 1);
}

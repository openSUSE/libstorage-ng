
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(disk1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    const Devicegraph* lhs = storage.get_probed();

    Devicegraph* rhs = storage.get_staging();

    Disk* sda = Disk::create(rhs, "/dev/sda");
    sda->set_size(16 * GiB);

    BOOST_CHECK_EXCEPTION(Actiongraph actiongraph(storage, lhs, rhs), Exception,
	[](const Exception& e) { return e.what() == string("cannot create disk"); }
    );
}


BOOST_AUTO_TEST_CASE(disk2)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::IMAGE);

    Storage storage(environment);

    const Devicegraph* lhs = storage.get_probed();

    Devicegraph* rhs = storage.get_staging();

    Disk* sda = Disk::create(rhs, "/dev/sda");
    sda->set_size(16 * GiB);

    Actiongraph actiongraph(storage, lhs, rhs);

    BOOST_CHECK_EQUAL(actiongraph.get_commit_actions().size(), 1);
}

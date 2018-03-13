
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/User.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"


using namespace storage;


BOOST_AUTO_TEST_CASE(copy_individual)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* a = storage.create_devicegraph("a");

    Disk* sda = Disk::create(a, "/dev/sda");
    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    User* user = to_user(a->find_holder(sda->get_sid(), gpt->get_sid()));

    BOOST_CHECK_EQUAL(a->num_devices(), 2);
    BOOST_CHECK_EQUAL(a->num_holders(), 1);

    Devicegraph* b = storage.create_devicegraph("b");

    sda->copy_to_devicegraph(b);
    gpt->copy_to_devicegraph(b);

    user->copy_to_devicegraph(b);

    BOOST_CHECK_EQUAL(b->num_devices(), 2);
    BOOST_CHECK_EQUAL(b->num_holders(), 1);

    BOOST_CHECK(storage.equal_devicegraph("a", "b"));

    storage.check();
}

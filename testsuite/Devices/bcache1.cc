
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Utils/HumanString.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Bcache.h"
#include "storage/Devices/BcacheCset.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(no_bcache_cset)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    sda->set_size(24 * TiB);

    Disk* nvme0n1 = Disk::create(staging, "/dev/nvme0n1");
    nvme0n1->set_size(2 * TiB);

    Bcache* bcache = sda->create_bcache("/dev/bcache0");

    BOOST_CHECK(bcache->get_backing_device() == sda);
    BOOST_CHECK(!bcache->has_bcache_cset());
    BOOST_CHECK_THROW(bcache->get_bcache_cset(), DeviceNotFound);

    BcacheCset* bcache_cset = nvme0n1->create_bcache_cset();
    bcache->attach_bcache_cset(bcache_cset);

    BOOST_CHECK(bcache->get_backing_device() == sda);
    BOOST_CHECK(bcache->has_bcache_cset());
    BOOST_CHECK(bcache->get_bcache_cset() == bcache_cset);
}

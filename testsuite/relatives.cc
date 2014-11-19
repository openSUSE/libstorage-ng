
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/Partition.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Devices/Ext4.h"
#include "storage/Devices/Swap.h"
#include "storage/DeviceGraph.h"


using namespace storage;


void
compare(vector<const Device*> a,  vector<const Device*> b)
{
    sort(a.begin(), a.end());
    sort(b.begin(), b.end());

    BOOST_CHECK(a == b); // TODO, error message useless
}


BOOST_AUTO_TEST_CASE(dependencies)
{
    DeviceGraph device_graph;

    Disk* sda = new Disk(device_graph, "/dev/sda");

    Partition* sda1 = new Partition(device_graph, "/dev/sda1");
    new Subdevice(device_graph, sda, sda1);

    Partition* sda2 = new Partition(device_graph, "/dev/sda2");
    new Subdevice(device_graph, sda, sda2);

    Disk* sdb = new Disk(device_graph, "/dev/sdb");

    Partition* sdb1 = new Partition(device_graph, "/dev/sdb1");
    new Subdevice(device_graph, sdb, sdb1);

    Partition* sdb2 = new Partition(device_graph, "/dev/sdb2");
    new Subdevice(device_graph, sdb, sdb2);

    LvmVg* system = new LvmVg(device_graph, "/dev/system");
    new Using(device_graph, sda1, system);
    new Using(device_graph, sdb1, system);

    LvmLv* system_root = new LvmLv(device_graph, "/dev/system/root");
    new Subdevice(device_graph, system, system_root);

    LvmLv* system_swap = new LvmLv(device_graph, "/dev/system/swap");
    new Subdevice(device_graph, system, system_swap);

    LvmLv* system_home = new LvmLv(device_graph, "/dev/system/home");
    new Subdevice(device_graph, system, system_home);

    BOOST_CHECK_EQUAL(num_vertices(device_graph.graph), 10);
    BOOST_CHECK_EQUAL(num_edges(device_graph.graph), 9);

    device_graph.check();

    compare(sda->getChildren(), { sda1, sda2 });

    compare(sda1->getParents(), { sda });

    compare(sda1->getSiblings(false), { sda2 });
    compare(system_swap->getSiblings(true), { system_root, system_home, system_swap });

    compare(sda->getDescendants(false), { sda1, sda2, system, system_root, system_home, system_swap });
    compare(system->getDescendants(true), { system, system_root, system_home, system_swap });

    compare(sda1->getAncestors(false), { sda });
    compare(system->getAncestors(true), { system, sda1, sda, sdb1, sdb });

    compare(sda->getLeafs(false), { sda2, system_root, system_swap, system_home });
    compare(system->getLeafs(false), { system_root, system_swap, system_home });

    compare(sda1->getRoots(false), { sda });
    compare(system_swap->getRoots(false), { sda, sdb });
}

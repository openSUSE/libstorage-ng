
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
#include "storage/Holders/Using.h"
#include "storage/Holders/Subdevice.h"
#include "storage/DeviceGraph.h"


using namespace storage;


vector<const Device*>
sort(const vector<const Device*>& devices)
{
    vector<const Device*> ret(devices.begin(), devices.end());
    sort(ret.begin(), ret.end());
    return ret;
}


namespace std
{
    ostream& operator<<(ostream& s, const vector<const Device*>& devices)
    {
	s << "{";
	for (vector<const Device*>::const_iterator it = devices.begin(); it != devices.end(); ++it)
	    s << (it == devices.begin() ? " " : ", ") << (*it)->display_name() << " (" << *it << ")";
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(dependencies)
{
    DeviceGraph* device_graph = new DeviceGraph();

    Disk* sda = Disk::create(device_graph, "/dev/sda");

    Partition* sda1 = Partition::create(device_graph, "/dev/sda1");
    Subdevice::create(device_graph, sda, sda1);

    Partition* sda2 = Partition::create(device_graph, "/dev/sda2");
    Subdevice::create(device_graph, sda, sda2);

    Disk* sdb = Disk::create(device_graph, "/dev/sdb");

    Partition* sdb1 = Partition::create(device_graph, "/dev/sdb1");
    Subdevice::create(device_graph, sdb, sdb1);

    Partition* sdb2 = Partition::create(device_graph, "/dev/sdb2");
    Subdevice::create(device_graph, sdb, sdb2);

    LvmVg* system = LvmVg::create(device_graph, "/dev/system");
    Using::create(device_graph, sda1, system);
    Using::create(device_graph, sdb1, system);

    LvmLv* system_root = LvmLv::create(device_graph, "/dev/system/root");
    Subdevice::create(device_graph, system, system_root);

    LvmLv* system_swap = LvmLv::create(device_graph, "/dev/system/swap");
    Subdevice::create(device_graph, system, system_swap);

    LvmLv* system_home = LvmLv::create(device_graph, "/dev/system/home");
    Subdevice::create(device_graph, system, system_home);

    BOOST_CHECK_EQUAL(device_graph->numDevices(), 10);
    BOOST_CHECK_EQUAL(device_graph->numHolders(), 9);

    device_graph->check();

    BOOST_CHECK_EQUAL(sort(sda->getChildren()), sort({ sda1, sda2 }));

    BOOST_CHECK_EQUAL(sort(sda1->getParents()), sort({ sda }));

    BOOST_CHECK_EQUAL(sort(sda1->getSiblings(false)), sort({ sda2 }));
    BOOST_CHECK_EQUAL(sort(system_swap->getSiblings(true)), sort({ system_root, system_home, system_swap }));

    BOOST_CHECK_EQUAL(sort(sda->getDescendants(false)), sort({ sda1, sda2, system, system_root, system_home, system_swap }));
    BOOST_CHECK_EQUAL(sort(system->getDescendants(true)), sort({ system, system_root, system_home, system_swap }));

    BOOST_CHECK_EQUAL(sort(sda1->getAncestors(false)), sort({ sda }));
    BOOST_CHECK_EQUAL(sort(system->getAncestors(true)), sort({ system, sda1, sda, sdb1, sdb }));

    BOOST_CHECK_EQUAL(sort(sda->getLeafs(false)), sort({ sda2, system_root, system_swap, system_home }));
    BOOST_CHECK_EQUAL(sort(system->getLeafs(false)), sort({ system_root, system_swap, system_home }));

    BOOST_CHECK_EQUAL(sort(sda1->getRoots(false)), sort({ sda }));
    BOOST_CHECK_EQUAL(sort(system_swap->getRoots(false)), sort({ sda, sdb }));

    delete device_graph;
}

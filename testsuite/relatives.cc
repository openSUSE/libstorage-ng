
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
#include "storage/Devicegraph.h"
#include "storage/Utils/Region.h"


using namespace std;
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
	    s << (it == devices.begin() ? " " : ", ") << (*it)->get_displayname() << " (" << *it << ")";
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(dependencies)
{
    Devicegraph* devicegraph = new Devicegraph();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");

    Partition* sda1 = Partition::create(devicegraph, "/dev/sda1", PRIMARY, Region(0, 100));
    Subdevice::create(devicegraph, sda, sda1);

    Partition* sda2 = Partition::create(devicegraph, "/dev/sda2", PRIMARY, Region(100, 100));
    Subdevice::create(devicegraph, sda, sda2);

    Disk* sdb = Disk::create(devicegraph, "/dev/sdb");

    Partition* sdb1 = Partition::create(devicegraph, "/dev/sdb1", PRIMARY, Region(0, 100));
    Subdevice::create(devicegraph, sdb, sdb1);

    Partition* sdb2 = Partition::create(devicegraph, "/dev/sdb2", PRIMARY, Region(100, 100));
    Subdevice::create(devicegraph, sdb, sdb2);

    LvmVg* system = LvmVg::create(devicegraph, "/dev/system");
    Using::create(devicegraph, sda1, system);
    Using::create(devicegraph, sdb1, system);

    LvmLv* system_root = LvmLv::create(devicegraph, "/dev/system/root");
    Subdevice::create(devicegraph, system, system_root);

    LvmLv* system_swap = LvmLv::create(devicegraph, "/dev/system/swap");
    Subdevice::create(devicegraph, system, system_swap);

    LvmLv* system_home = LvmLv::create(devicegraph, "/dev/system/home");
    Subdevice::create(devicegraph, system, system_home);

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 10);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 9);

    devicegraph->check();

    BOOST_CHECK_EQUAL(sort(sda->get_children()), sort({ sda1, sda2 }));

    BOOST_CHECK_EQUAL(sort(sda1->get_parents()), sort({ sda }));

    BOOST_CHECK_EQUAL(sort(sda1->get_siblings(false)), sort({ sda2 }));
    BOOST_CHECK_EQUAL(sort(system_swap->get_siblings(true)), sort({ system_root, system_home, system_swap }));

    BOOST_CHECK_EQUAL(sort(sda->get_descendants(false)), sort({ sda1, sda2, system, system_root, system_home, system_swap }));
    BOOST_CHECK_EQUAL(sort(system->get_descendants(true)), sort({ system, system_root, system_home, system_swap }));

    BOOST_CHECK_EQUAL(sort(sda1->get_ancestors(false)), sort({ sda }));
    BOOST_CHECK_EQUAL(sort(system->get_ancestors(true)), sort({ system, sda1, sda, sdb1, sdb }));

    BOOST_CHECK_EQUAL(sort(sda->get_leafs(false)), sort({ sda2, system_root, system_swap, system_home }));
    BOOST_CHECK_EQUAL(sort(system->get_leafs(false)), sort({ system_root, system_swap, system_home }));

    BOOST_CHECK_EQUAL(sort(sda1->get_roots(false)), sort({ sda }));
    BOOST_CHECK_EQUAL(sort(system_swap->get_roots(false)), sort({ sda, sdb }));

    delete devicegraph;
}


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Disk.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<Disk*>& disks)
    {
	s << "{";
	for (vector<Disk*>::const_iterator it = disks.begin(); it != disks.end(); ++it)
	    s << (it == disks.begin() ? " " : ", ") << (*it)->get_displayname() << " (" << *it << ")";
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(disk_sorting1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Disk* sda = Disk::create(staging, "/dev/sda");
    Disk* sdz = Disk::create(staging, "/dev/sdz");
    Disk* sdaa = Disk::create(staging, "/dev/sdaa");

    Disk* vda = Disk::create(staging, "/dev/vda");
    Disk* vdb = Disk::create(staging, "/dev/vdb");
    Disk* vdaa = Disk::create(staging, "/dev/vdaa");

    Disk* pmem0 = Disk::create(staging, "/dev/pmem0");
    Disk* pmem10 = Disk::create(staging, "/dev/pmem10");
    Disk* pmem2 = Disk::create(staging, "/dev/pmem2");

    vector<Disk*> all = Disk::get_all(staging);
    sort(all.begin(), all.end(), Disk::compare_by_name);

    BOOST_CHECK_EQUAL(all, vector<Disk*>({
	pmem0, pmem2, pmem10, sda, sdz, sdaa, vda, vdb, vdaa
    }));
}

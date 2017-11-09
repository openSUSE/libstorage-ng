
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/Disk.h"
#include "storage/Devices/Dasd.h"
#include "storage/Devices/Md.h"
#include "storage/Devices/Multipath.h"
#include "storage/Devices/DmRaid.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<Partitionable*>& partitionables)
    {
	s << "{";
	for (vector<Partitionable*>::const_iterator it = partitionables.begin(); it != partitionables.end(); ++it)
	    s << (it == partitionables.begin() ? " " : ", ") << (*it)->get_displayname() << " (" << *it << ")";
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(partitionable_sorting1)
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

    Dasd* dasda = Dasd::create(staging, "/dev/dasda");
    Dasd* dasdb = Dasd::create(staging, "/dev/dasdb");
    Dasd* dasdaa = Dasd::create(staging, "/dev/dasdaa");

    Md* md10 = Md::create(staging, "/dev/md10");
    Md* md0 = Md::create(staging, "/dev/md0");
    Md* md1 = Md::create(staging, "/dev/md1");

    Md* md_foo = Md::create(staging, "/dev/md/foo");
    Md* md_bar = Md::create(staging, "/dev/md/bar");

    Md* md_2 = Md::create(staging, "/dev/md/2");

    Multipath* mpath_1 = Multipath::create(staging, "/dev/mapper/36005076305ffc73a00000000000013b4");

    DmRaid* dm_raid_1 = DmRaid::create(staging, "/dev/mapper/isw_ddgdcbibhd_test1");

    vector<Partitionable*> all = Partitionable::get_all(staging);
    sort(all.begin(), all.end(), Partitionable::compare_by_name);

    BOOST_CHECK_EQUAL(all, vector<Partitionable*>({
	dasda, dasdb, dasdaa, mpath_1, dm_raid_1, md_2, md_bar, md_foo, md0, md1, md10, pmem0,
	pmem2, pmem10, sda, sdz, sdaa, vda, vdb, vdaa
    }));
}

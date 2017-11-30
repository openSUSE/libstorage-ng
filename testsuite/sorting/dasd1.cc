
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Dasd.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<Dasd*>& dasds)
    {
	s << "{";
	for (vector<Dasd*>::const_iterator it = dasds.begin(); it != dasds.end(); ++it)
	    s << (it == dasds.begin() ? " " : ", ") << (*it)->get_displayname();
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(dasd_sorting1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Dasd* dasda = Dasd::create(staging, "/dev/dasda");
    Dasd* dasdz = Dasd::create(staging, "/dev/dasdz");
    Dasd* dasdaa = Dasd::create(staging, "/dev/dasdaa");

    Dasd* vda = Dasd::create(staging, "/dev/vda");
    Dasd* vdb = Dasd::create(staging, "/dev/vdb");
    Dasd* vdaa = Dasd::create(staging, "/dev/vdaa");

    vector<Dasd*> all = Dasd::get_all(staging);
    sort(all.begin(), all.end(), Dasd::compare_by_name);

    BOOST_CHECK_EQUAL(all, vector<Dasd*>({
	dasda, dasdz, dasdaa, vda, vdb, vdaa
    }));
}

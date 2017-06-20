
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/Md.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<Md*>& mds)
    {
        s << "{";
        for (vector<Md*>::const_iterator it = mds.begin(); it != mds.end(); ++it)
            s << (it == mds.begin() ? " " : ", ") << (*it)->get_displayname() << " (" << *it << ")";
        s << " }";

        return s;
    }
}


BOOST_AUTO_TEST_CASE(md_sorting1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    Md* md10 = Md::create(staging, "/dev/md10");
    Md* md0 = Md::create(staging, "/dev/md0");
    Md* md2 = Md::create(staging, "/dev/md2");
    Md* md1 = Md::create(staging, "/dev/md1");

    Md* foo = Md::create(staging, "/dev/md/foo");
    Md* bar = Md::create(staging, "/dev/md/bar");

    BOOST_CHECK_EQUAL(staging->get_all_mds(), vector<Md*>({ bar, foo, md0, md1, md2, md10 }));
}

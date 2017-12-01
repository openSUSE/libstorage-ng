
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/LvmVg.h"
#include "storage/Devices/LvmLv.h"
#include "storage/Utils/HumanString.h"


using namespace std;
using namespace storage;


namespace std
{
    ostream& operator<<(ostream& s, const vector<Device*>& devices)
    {
	s << "{";
	for (vector<Device*>::const_iterator it = devices.begin(); it != devices.end(); ++it)
	    s << (it == devices.begin() ? " " : ", ") << (*it)->get_displayname();
	s << " }";

	return s;
    }
}


BOOST_AUTO_TEST_CASE(device_sorting1)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* staging = storage.get_staging();

    LvmVg* system = LvmVg::create(staging, "system");
    LvmLv* system_root = system->create_lvm_lv("root", LvType::NORMAL, 20 * GiB);
    LvmLv* system_swap = system->create_lvm_lv("swap", LvType::NORMAL, 2 * GiB);
    LvmLv* system_home = system->create_lvm_lv("home", LvType::NORMAL, 500 * GiB);

    vector<Device*> all = Device::get_all(staging);
    sort(all.begin(), all.end(), Device::compare_by_name);

    BOOST_CHECK_EQUAL(all, vector<Device*>({
	system, system_home, system_root, system_swap
    }));
}

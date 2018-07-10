
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Environment.h"
#include "storage/Storage.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/Mockup.h"
#include "storage/Utils/StorageDefines.h"


using namespace std;
using namespace storage;


BOOST_AUTO_TEST_CASE(find_vertex)
{
    set_logger(get_stdout_logger());

    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* devicegraph = storage.get_staging();

    Disk* sda = Disk::create(devicegraph, "/dev/sda");
    sda->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda");
    sda->get_impl().set_sysfs_name("sda");

    Gpt* gpt = to_gpt(sda->create_partition_table(PtType::GPT));

    Partition* sda1 = gpt->create_partition("/dev/sda1", Region(2048, 1000000, 512), PartitionType::PRIMARY);
    sda1->get_impl().set_sysfs_path("/devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1");
    sda1->get_impl().set_sysfs_name("sda1");

    BOOST_CHECK_EQUAL(devicegraph->num_devices(), 3);
    BOOST_CHECK_EQUAL(devicegraph->num_holders(), 2);

    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda"), sda);
    BOOST_CHECK_EQUAL(BlkDevice::find_by_name(devicegraph, "/dev/sda1"), sda1);
    BOOST_CHECK_THROW(BlkDevice::find_by_name(devicegraph, "/dev/sda2"), DeviceNotFound);

    BOOST_CHECK(sda->exists_in_devicegraph(devicegraph));
    BOOST_CHECK(!sda->exists_in_probed());
    BOOST_CHECK(sda->exists_in_staging());

    // For testing find_by_any_name some mockup is needed since
    // find_by_any_name can do a system lookup. find_by_any_name also only
    // works on the probed and system devicegraph.

    Mockup::set_mode(Mockup::Mode::PLAYBACK);

    Mockup::set_command(UDEVADMBIN " settle --timeout=20", vector<string>({}));

    storage.remove_devicegraph("system");
    storage.copy_devicegraph("staging", "system");

    const Devicegraph* system = storage.get_system();

    // Looking up a device by the name libstorage-ng knows it does not need
    // udevadm info calls.

    BOOST_CHECK_EQUAL(BlkDevice::find_by_any_name(system, "/dev/sda1")->get_sid(), sda1->get_sid());

    // Looking up a device by another name needs udevadm info calls.

    Mockup::set_command(UDEVADMBIN " info '/dev/block/8:1'", vector<string>({
	"P: /devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda1",
	"N: sda1"
    }));

    BOOST_CHECK_EQUAL(BlkDevice::find_by_any_name(system, "/dev/block/8:1")->get_sid(), sda1->get_sid());

    Mockup::set_command(UDEVADMBIN " info '/dev/block/8:2'", vector<string>({
	"P: /devices/pci0000:00/0000:00:1f.2/ata1/host0/target0:0:0/0:0:0:0/block/sda/sda2",
	"N: sda2"
    }));

    BOOST_CHECK_THROW(BlkDevice::find_by_any_name(system, "/dev/block/8:2"), DeviceNotFound);

    // Looking up a device unknown to udevadm info throws DeviceNotFound.

    Mockup::set_command(UDEVADMBIN " info '/dev/does-not-exist'",
			RemoteCommand( vector<string>({}), vector<string>({ "Unknown device, [...]" }), 4));

    BOOST_CHECK_THROW(BlkDevice::find_by_any_name(system, "/dev/does-not-exist"), DeviceNotFound);
}

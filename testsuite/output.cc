
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <boost/algorithm/string.hpp>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devices/Partition.h"
#include "storage/Devicegraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"
#include "storage/Devices/DeviceImpl.h"


using namespace std;
using namespace storage;


class Fixture
{
public:

    Fixture()
	: storage(nullptr)
    {
	Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

	storage = new Storage(environment);

	Devicegraph* devicegraph = storage->get_staging();

	Disk* sda = Disk::create(devicegraph, "/dev/sda");
	sda->set_size_k(80 * 1024 * 1024);
	sda->get_impl().set_transport(Transport::SATA);

	PartitionTable* gpt = sda->create_partition_table(PtType::GPT);

	Partition* sda1 = gpt->create_partition("/dev/sda1", Region(0, 0, 262144), PartitionType::PRIMARY);
	sda1->set_size_k(1024 * 1024);
	sda1->set_id(ID_LINUX);
    }

    ~Fixture()
    {
	delete storage;
    }

    const Storage* get_storage() const { return storage; }

private:

    Storage* storage;

};


// A Boost UTF Global Fixture gives no access to the fixture object. So simply
// have my own object.
Fixture fixture;


BOOST_AUTO_TEST_CASE(test_disk)
{
    string expected = "Disk sid:42 displayname:/dev/sda name:/dev/sda size-k:83886080 geometry:[0, 32, 16, 512 B] transport:SATA";

    ostringstream out;
    out << *(Disk::find(fixture.get_storage()->get_staging(), "/dev/sda")) << endl;

    BOOST_CHECK_EQUAL(out.str(), expected + "\n");
}


BOOST_AUTO_TEST_CASE(test_partition)
{
    string expected = "Partition sid:44 displayname:/dev/sda1 name:/dev/sda1 size-k:1048576 region:[0, 4096, 262144 B]";

    ostringstream out;
    out << *(Partition::find(fixture.get_storage()->get_staging(), "/dev/sda1")) << endl;

    BOOST_CHECK_EQUAL(out.str(), expected + "\n");
}


BOOST_AUTO_TEST_CASE(test_devicegraph)
{
    list<string> expected = {
	"Disk sid:42 displayname:/dev/sda name:/dev/sda size-k:83886080 geometry:[0, 32, 16, 512 B] transport:SATA --> 43",
	"Gpt sid:43 displayname:gpt --> 44",
	"Partition sid:44 displayname:/dev/sda1 name:/dev/sda1 size-k:1048576 region:[0, 4096, 262144 B] -->",
	"User source-sid:42 target-sid:43",
	"Subdevice source-sid:43 target-sid:44"
    };

    ostringstream out;
    out << *(fixture.get_storage()->get_staging());

    BOOST_CHECK_EQUAL(out.str(), boost::join(expected, "\n") + "\n");
}

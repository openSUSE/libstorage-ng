
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE libstorage

#include <sstream>
#include <boost/test/unit_test.hpp>

#include "storage/Devices/Disk.h"
#include "storage/Devices/PartitionTable.h"
#include "storage/Devicegraph.h"
#include "storage/Actiongraph.h"
#include "storage/Storage.h"
#include "storage/Environment.h"


using namespace std;
using namespace storage;


string
disk_name(int i)
{
    ostringstream s;
    s << "/dev/disk" << i;
    return s.str();
}


string
partition_name(int i, int j)
{
    ostringstream s;
    s << "/dev/disk" << i << "p" << j;
    return s.str();
}


void
add_disk(Devicegraph* devicegraph, int i)
{
    Disk::create(devicegraph, disk_name(i));
}


void
add_partitions(Devicegraph* devicegraph, int i)
{
    Disk* disk = Disk::find_by_name(devicegraph, disk_name(i));

    PartitionTable* partition_table = disk->create_partition_table(PtType::GPT);

    for (int j = 1; j < 5; ++j)
	partition_table->create_partition(partition_name(i, j), Region(1000 * j, 1000 * (j + 1), 512),
					  PartitionType::PRIMARY);
}


BOOST_AUTO_TEST_CASE(performance)
{
    Environment environment(true, ProbeMode::NONE, TargetMode::DIRECT);

    Storage storage(environment);

    Devicegraph* lhs = storage.create_devicegraph("lhs");

    const int n = 1000;

    for (int i = 0; i < n; ++i)
	add_disk(lhs, i);

    Devicegraph* rhs = storage.copy_devicegraph("lhs", "rhs");

    for (int i = 0; i < n; ++i)
	add_partitions(rhs, i);

    Actiongraph actiongraph(storage, lhs, rhs);

    // TODO actually fail if too slow? how can that be done stable?
}

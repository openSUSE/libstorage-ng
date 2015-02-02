

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const vector<string> EnumInfo<PtType>::names({
	"unknown", "loop", "msdos", "gpt", "dasd", "mac"
    });


    PartitionTable::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), read_only(false)
    {
    }


    void
    PartitionTable::Impl::probe(SystemInfo& systeminfo)
    {
	Device::Impl::probe(systeminfo);

	const Devicegraph* g = get_devicegraph();

	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());

	const Disk* disk = to_disk(g->get_impl().graph[v1].get());

	const Parted& parted = systeminfo.getParted(disk->get_name());

	if (parted.getImplicit())
	    read_only = true;

	for (const Parted::Entry& entry : parted.getEntries())
	{
	    string name = disk->get_impl().partition_name(entry.num);
	    Partition* p = create_partition(name, entry.type);
	    p->get_impl().probe(systeminfo);
	}
    }


    void
    PartitionTable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "read-only", read_only, read_only);
    }


    Partition*
    PartitionTable::Impl::create_partition(const string& name, PartitionType type)
    {
	Partition* partition = Partition::create(get_devicegraph(), name, type);
	Subdevice::create(get_devicegraph(), get_device(), partition);

	return partition;
    }


    Partition*
    PartitionTable::Impl::get_partition(const string& name)
    {
	Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO

	for (Partition* partition : devicegraph->get_impl().getDevices<Partition>(devicegraph->get_impl().children(vertex)))
	{
	    if (partition->get_name() == name)
	    {
		return partition;
	    }
	}

	throw runtime_error("partition not found");
    }


    void
    PartitionTable::Impl::delete_partition(const string& name)
    {
	Partition* partition = get_partition(name);
	assert(partition);

	Devicegraph* devicegraph = get_devicegraph();

	devicegraph->remove_device(partition);
    }


    Disk*
    PartitionTable::Impl::get_disk()
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_disk(get_devicegraph()->get_impl().graph[v].get());
    }


    const Disk*
    PartitionTable::Impl::get_disk() const
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_disk(get_devicegraph()->get_impl().graph[v].get());
    }


    vector<const Partition*>
    PartitionTable::Impl::get_partitions() const
    {
	const Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().getDevices<Partition>(devicegraph->get_impl().children(vertex));
    }


    bool
    PartitionTable::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return read_only == rhs.read_only;
    }


    void
    PartitionTable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "read-only", read_only, rhs.read_only);
    }


    void
    PartitionTable::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);
    }


    unsigned int
    PartitionTable::Impl::num_primary() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PRIMARY;
	});
    }


    bool
    PartitionTable::Impl::has_extended() const
    {
	vector<const Partition*> partitions = get_partitions();
	return any_of(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == EXTENDED;
	});
    }


    unsigned int
    PartitionTable::Impl::num_logical() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == LOGICAL;
	});
    }


    Text
    PartitionTable::Impl::do_create_text(bool doing) const
    {
	const Disk* disk = get_disk();

	return sformat(_("Create %1$s on %2$s"), get_displayname().c_str(),
		       disk->get_displayname().c_str());
    }

}

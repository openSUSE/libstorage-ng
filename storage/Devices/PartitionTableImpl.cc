

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

	string pp_name = to_blkdevice(g->get_impl().graph[v1].get())->get_name();

	const Parted& parted = systeminfo.getParted(pp_name);

	if (parted.getImplicit())
	    read_only = true;

	for (const Parted::Entry& entry : parted.getEntries())
	{
	    Partition* p = create_partition(entry.num);
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
    PartitionTable::Impl::create_partition(const string& name)
    {
	Partition* partition = Partition::create(get_devicegraph(), name);
	Subdevice::create(get_devicegraph(), get_device(), partition);

	return partition;
    }


    Partition*
    PartitionTable::Impl::create_partition(unsigned int number)
    {
	Disk* disk = get_disk();

	string disk_name = disk->get_name();

	Partition* partition = Partition::create(get_devicegraph(), disk_name + decString(number)); // TODO

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

	devicegraph->remove_vertex(partition->get_sid());
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

}

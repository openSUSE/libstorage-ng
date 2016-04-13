

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    const vector<string> EnumTraits<PtType>::names({
	"unknown", "loop", "MS-DOS", "GPT", "DASD", "Mac"
    });


    const char* DeviceTraits<PartitionTable>::classname = "PartitionTable";


    PartitionTable::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), read_only(false)
    {
    }


    void
    PartitionTable::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Device::Impl::probe_pass_1(probed, systeminfo);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = systeminfo.getParted(partitionable->get_name());

	if (parted.getImplicit())
	    read_only = true;

	for (const Parted::Entry& entry : parted.getEntries())
	{
	    string name = partitionable->get_impl().partition_name(entry.num);
	    Partition* p = create_partition(name, entry.region, entry.type);
	    p->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    PartitionTable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "read-only", read_only, read_only);
    }


    Partition*
    PartitionTable::Impl::create_partition(const string& name, const Region& region, PartitionType type)
    {
	const Region& partitionable_region = get_partitionable()->get_impl().get_region();
	if (region.get_block_size() != partitionable_region.get_block_size())
	    ST_THROW(DifferentBlockSizes(region.get_block_size(), partitionable_region.get_block_size()));

	Partition* partition = Partition::create(get_devicegraph(), name, region, type);
	Subdevice::create(get_devicegraph(), get_device(), partition);

	return partition;
    }


    Partition*
    PartitionTable::Impl::get_partition(const string& name)
    {
	Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	// TODO

	for (Partition* partition : devicegraph->get_impl().filter_devices_of_type<Partition>(devicegraph->get_impl().children(vertex)))
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

	Devicegraph* devicegraph = get_devicegraph();

	devicegraph->remove_device(partition);
    }


    Partitionable*
    PartitionTable::Impl::get_partitionable()
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[v]);
    }


    const Partitionable*
    PartitionTable::Impl::get_partitionable() const
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[v]);
    }


    vector<Partition*>
    PartitionTable::Impl::get_partitions()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<Partition>(devicegraph.children(vertex),
							     compare_by_number);
    }


    vector<const Partition*>
    PartitionTable::Impl::get_partitions() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<Partition>(devicegraph.children(vertex),
							     compare_by_number);
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
	    return partition->get_type() == PartitionType::PRIMARY;
	});
    }


    const Partition*
    PartitionTable::Impl::get_extended() const
    {
	vector<const Partition*> partitions = get_partitions();
	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == PartitionType::EXTENDED)
		return partition;
	}

	throw runtime_error("has no extended partition");
    }


    vector<PartitionSlot>
    PartitionTable::Impl::get_unused_partition_slots(bool all, bool logical, AlignPolicy align_policy) const
    {
	y2mil("all:" << all << " logical:" << logical);

	const Partitionable* partitionable = get_partitionable();
	const Topology& topology = partitionable->get_topology();

	const Region& region = partitionable->get_impl().get_region();

	bool tmp_primary_possible = num_primary() + (has_extended() ? 1 : 0) < max_primary();
	bool tmp_extended_possible = tmp_primary_possible && extended_possible() && !has_extended();
	bool tmp_logical_possible = has_extended() && num_logical() < (max_logical() - max_primary());

	vector<PartitionSlot> slots;

	vector<const Partition*> partitions = get_partitions();
	sort(partitions.begin(), partitions.end(), compare_by_number);

	if (all || !logical)
	{
	    PartitionSlot slot;

	    if (true /* label != "dasd" */)
	    {
		vector<const Partition*>::const_iterator it = partitions.begin();
		unsigned start = 1; // label != "mac" ? 1 : 2;
		while (it != partitions.end() && (*it)->get_number() <= start &&
		       (*it)->get_number() <= max_primary())
		{
		    if ((*it)->get_number() == start)
			++start;
		    /*
		    if (label == "sun" && start == 3)
		        ++start;
		    */
		    ++it;
		}
		slot.nr = start;
	    }
	    else
	    {
		slot.nr = 1;
	    }

	    slot.name = partitionable->get_impl().partition_name(slot.nr);

	    slot.primary_slot = true;
	    slot.primary_possible = tmp_primary_possible;
	    slot.extended_slot = true;
	    slot.extended_possible = tmp_extended_possible;
	    slot.logical_slot = false;
	    slot.logical_possible = false;

	    unsigned long start = 0;
	    unsigned long end = region.get_length();

	    list<Region> tmp;
	    for (const Partition* partition : partitions)
	    {
		if (partition->get_type() != PartitionType::LOGICAL)
		    tmp.push_back(partition->get_region());
	    }
	    tmp.sort();

	    for (list<Region>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
	    {
		if (i->get_start() > start)
		{
		    slot.region = Region(start, i->get_start() - start, region.get_block_size());
		    slots.push_back(slot);
		}
		start = i->get_end() + 1;

		/*
		if (label == "dasd")
		{
		    slot.nr++;
		    slot.device = getPartDevice(slot.nr);
		}
		*/
	    }
	    if (end > start)
	    {
		slot.region = Region(start, end - start, region.get_block_size());
		slots.push_back(slot);
	    }
	}

	if (all || logical)
	{
	    try
	    {
		const Partition* extended = get_extended();

		PartitionSlot slot;

		slot.nr = max_primary() + num_logical() + 1;
		slot.name = partitionable->get_impl().partition_name(slot.nr);

		slot.primary_slot = false;
		slot.primary_possible = false;
		slot.extended_slot = false;
		slot.extended_possible = false;
		slot.logical_slot = true;
		slot.logical_possible = tmp_logical_possible;

		unsigned long start = extended->get_region().get_start();
		unsigned long end = extended->get_region().get_end();

		list<Region> tmp;
		for (const Partition* partition : partitions)
		{
		    if (partition->get_type() == PartitionType::LOGICAL)
			tmp.push_back(partition->get_region());
		}
		tmp.sort();

		for (list<Region>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
		{
		    if (i->get_start() > start)
		    {
			slot.region = Region(start, i->get_start() - start, region.get_block_size());
			slots.push_back(slot);
		    }
		    start = i->get_end() + 1;
		}
		if (end > start)
		{
		    slot.region = Region(start, end - start, region.get_block_size());
		    slots.push_back(slot);
		}
	    }
	    catch (...)
	    {
	    }
	}

	y2deb("slots:" << slots);

	Region usable_region = get_usable_region();

	vector<PartitionSlot>::iterator it = slots.begin();
	while (it != slots.end())
	{
	    if (usable_region.intersect(it->region))
	    {
		it->region = usable_region.intersection(it->region);

		it->region = topology.align(it->region, align_policy);

		if (it->region.get_length() > 0) // TODO
		{
		    ++it;
		}
		else
		{
		    it = slots.erase(it);
		}
	    }
	    else
	    {
		it = slots.erase(it);
	    }
	}

	y2mil("slots:" << slots);

	return slots;
    }


    Region
    PartitionTable::Impl::align(const Region& region, AlignPolicy align_policy) const
    {
	return get_partitionable()->get_topology().align(region, align_policy);
    }


    std::ostream&
    operator<<(std::ostream& s, const PartitionSlot& partition_slot)
    {
	s << "region:" << partition_slot.region << " nr:" << partition_slot.nr
	  << " name:" << partition_slot.name
	  << " primary_slot:" << partition_slot.primary_slot
	  << " primary_possible:" << partition_slot.primary_possible
	  << " extended_slot:" << partition_slot.extended_slot
	  << " extended_possible:" << partition_slot.extended_possible
	  << " logical_slot:" << partition_slot.logical_slot
	  << " logical_possible:" << partition_slot.logical_possible;

	return s;
    }

}

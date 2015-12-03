

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


    const vector<string> EnumTraits<PtType>::names({
	"unknown", "loop", "msdos", "gpt", "dasd", "mac"
    });


    const char* DeviceTraits<PartitionTable>::classname = "PartitionTable";


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

	const Disk* disk = to_disk(g->get_impl()[v1]);

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

	Devicegraph* devicegraph = get_devicegraph();

	devicegraph->remove_device(partition);
    }


    Disk*
    PartitionTable::Impl::get_disk()
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_disk(get_devicegraph()->get_impl()[v]);
    }


    const Disk*
    PartitionTable::Impl::get_disk() const
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return to_disk(get_devicegraph()->get_impl()[v]);
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


    const Partition*
    PartitionTable::Impl::get_extended() const
    {
	vector<const Partition*> partitions = get_partitions();
	for (const Partition* partition : partitions)
	{
	    if (partition->get_type() == EXTENDED)
		return partition;
	}

	throw runtime_error("has no extended partition");
    }


    Region
    PartitionTable::Impl::get_usable_region() const
    {
	const Geometry& geometry = get_disk()->get_impl().get_geometry();

	return Region(0, geometry.cylinders);
    }


    list<PartitionSlotInfo>
    PartitionTable::Impl::get_unused_partition_slots(bool all, bool logical) const
    {
	y2mil("all:" << all << " logical:" << logical);

	unsigned int range = get_disk()->get_impl().get_range();

	bool tmp_primary_possible = num_primary() + (has_extended() ? 1 : 0) < max_primary(range);
	bool tmp_extended_possible = tmp_primary_possible && extended_possible() && !has_extended();
	bool tmp_logical_possible = has_extended() && num_logical() < (max_logical(range) - max_primary(range));

	list<PartitionSlotInfo> slots;

	vector<const Partition*> partitions = get_partitions();
	sort(partitions.begin(), partitions.end(), Partition::Impl::cmp_lt_number);

	if (all || !logical)
	{
	    PartitionSlotInfo slot;

	    if (true /* label != "dasd" */)
	    {
		vector<const Partition*>::const_iterator it = partitions.begin();
		unsigned start = 1; // label != "mac" ? 1 : 2;
		while (it != partitions.end() && (*it)->get_number() <= start &&
		       (*it)->get_number() <= max_primary(range))
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

	    slot.device = get_disk()->get_impl().partition_name(slot.nr);

	    slot.primarySlot = true;
	    slot.primaryPossible = tmp_primary_possible;
	    slot.extendedSlot = true;
	    slot.extendedPossible = tmp_extended_possible;
	    slot.logicalSlot = false;
	    slot.logicalPossible = false;

	    unsigned long start = 0;
	    unsigned long end = get_disk()->get_impl().get_geometry().cylinders;

	    list<Region> tmp;
	    for (const Partition* partition : partitions)
	    {
		if (partition->get_type() != LOGICAL)
		    tmp.push_back(partition->get_region());
	    }
	    tmp.sort();

	    for (list<Region>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
	    {
		if (i->get_start() > start)
		{
		    slot.cylRegion = RegionInfo(start, i->get_start() - start);
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
		slot.cylRegion = RegionInfo(start, end - start);
		slots.push_back(slot);
	    }
	}

	if (all || logical)
	{
	    try
	    {
		const Partition* extended = get_extended();

		PartitionSlotInfo slot;

		slot.nr = max_primary(range) + num_logical() + 1;
		slot.device = get_disk()->get_impl().partition_name(slot.nr);

		slot.primarySlot = false;
		slot.primaryPossible = false;
		slot.extendedSlot = false;
		slot.extendedPossible = false;
		slot.logicalSlot = true;
		slot.logicalPossible = tmp_logical_possible;

		unsigned long start = extended->get_region().get_start();
		unsigned long end = extended->get_region().get_end();

		list<Region> tmp;
		for (const Partition* partition : partitions)
		{
		    if (partition->get_type() == LOGICAL)
			tmp.push_back(partition->get_region());
		}
		tmp.sort();

		for (list<Region>::const_iterator i = tmp.begin(); i != tmp.end(); ++i)
		{
		    if (i->get_start() > start)
		    {
			slot.cylRegion = RegionInfo(start, i->get_start() - start);
			slots.push_back(slot);
		    }
		    start = i->get_end() + 1;
		}
		if (end > start)
		{
		    slot.cylRegion = RegionInfo(start, end - start);
		    slots.push_back(slot);
		}
	    }
	    catch (...)
	    {
	    }
	}

	y2deb("slots:" << slots);

	Region usable_region = get_usable_region();

	list<PartitionSlotInfo>::iterator it = slots.begin();
	while (it != slots.end())
	{
	    if (usable_region.intersect(it->cylRegion))
	    {
		it->cylRegion = usable_region.intersection(it->cylRegion);
		++it;
	    }
	    else
	    {
		it = slots.erase(it);
	    }
	}

	y2deb("slots:" << slots);

	return slots;
    }


    std::ostream&
    operator<<(std::ostream& s, const PartitionSlotInfo& a)
    {
	s << "region:" << Region(a.cylRegion) << " nr:" << a.nr << " device:" << a.device
	  << " primary_slot:" << a.primarySlot << " primary_possible:" << a.primaryPossible
	  << " extended_slot:" << a.extendedSlot << " extended_possible:" << a.extendedPossible
	  << " logical_slot:" << a.logicalSlot << " logical_possible:" << a.logicalPossible;
	return s;
    }

}

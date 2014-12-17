

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

	string pp_name = dynamic_cast<const BlkDevice*>(g->get_impl().graph[v1].get())->get_name();

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
	Devicegraph::Impl::vertex_descriptor v1 = get_devicegraph()->get_impl().parent(get_vertex());

	string pp_name = dynamic_cast<const BlkDevice*>(get_devicegraph()->get_impl().graph[v1].get())->get_name();

	Partition* partition = Partition::create(get_devicegraph(), pp_name + decString(number));
	Subdevice::create(get_devicegraph(), get_device(), partition);

	return partition;
    }

}

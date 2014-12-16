

#include "storage/Devices/DiskImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Holders/Using.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/Enum.h"


namespace storage_bgl
{

    using namespace std;


    Disk::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), transport(storage::TUNKNOWN)
    {
	string tmp;

	if (getChildValue(node, "transport", tmp))
	    transport = toValueWithFallback(tmp, storage::TUNKNOWN);
    }


    void
    Disk::Impl::probe(SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe(systeminfo);

	Lsscsi::Entry entry;
	if (systeminfo.getLsscsi().getEntry(get_name(), entry))
	    transport = entry.transport;

	const Parted& parted = systeminfo.getParted(get_name());
	if (parted.getLabel() == PtType::MSDOS || parted.getLabel() == PtType::GPT)
	{
	    PartitionTable* pt = create_partition_table(parted.getLabel());
	    pt->get_impl().probe(systeminfo);
	}
    }


    void
    Disk::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValueIf(node, "transport", toString(transport), transport != storage::TUNKNOWN);
    }


    PartitionTable*
    Disk::Impl::create_partition_table(PtType pt_type)
    {
	if (get_device()->num_children() != 0)
	    throw runtime_error("has children");

	PartitionTable* ret = nullptr;

	switch (pt_type)
	{
	    case PtType::MSDOS:
		ret = Msdos::create(get_devicegraph());
		break;

	    case PtType::GPT:
		ret = Gpt::create(get_devicegraph());
		break;

	    default:
		throw;
	}

	Using::create(get_devicegraph(), get_device(), ret);
	return ret;
    }


    PartitionTable*
    Disk::Impl::get_partition_table()
    {
	if (get_device()->num_children() != 1)
	    throw runtime_error("has no children");

	const Devicegraph* devicegraph = get_devicegraph();

	Device* child = devicegraph->get_impl().graph[devicegraph->get_impl().child(get_vertex())].get();

	return dynamic_cast<PartitionTable*>(child);
    }


    void
    Disk::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	throw runtime_error("cannot create disk");
    }


    void
    Disk::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	throw runtime_error("cannot delete disk");
    }

}

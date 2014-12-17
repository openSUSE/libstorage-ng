

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), boot(false)
    {
	string tmp;

	getChildValue(node, "region", region);
	if (getChildValue(node, "partition_type", tmp))
            type = toValueWithFallback(tmp, PRIMARY);
	getChildValue(node, "id", id);
	getChildValue(node, "boot", boot);
    }


    void
    Partition::Impl::probe(SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe(systeminfo);

	const Devicegraph* g = get_devicegraph();

	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());
	Devicegraph::Impl::vertex_descriptor v2 = g->get_impl().parent(v1);

	string pp_name = dynamic_cast<const BlkDevice*>(g->get_impl().graph[v2].get())->get_name();

	const Parted& parted = systeminfo.getParted(pp_name);
	Parted::Entry entry;
	if (!parted.getEntry(get_number(), entry))
	    throw;

	region = entry.cylRegion;
	type = entry.type;
	id = entry.id;
	boot = entry.boot;
    }


    void
    Partition::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "region", region);
	setChildValue(node, "type", toString(type));
	setChildValueIf(node, "id", id, id != 0);
	setChildValueIf(node, "boot", boot, boot);
    }


    unsigned int
    Partition::Impl::get_number() const
    {
	string::size_type pos = get_name().find_last_not_of("0123456789");
	return atoi(get_name().substr(pos + 1).c_str());
    }


    void
    Partition::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::SetType(get_sid()));

	actiongraph.add_chain(actions);
    }

}

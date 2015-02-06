

#include <iostream>

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), region(), type(PRIMARY), id(ID_LINUX), boot(false)
    {
	string tmp;

	getChildValue(node, "region", region);
	if (getChildValue(node, "type", tmp))
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
    Partition::Impl::set_size_k(unsigned long long size_k)
    {
	BlkDevice::Impl::set_size_k(size_k);

	const PartitionTable* partitiontable = get_partition_table();
	assert(partitiontable);

	const Disk* disk = partitiontable->get_disk();
	assert(disk);

	region.set_length(get_size_k() * 1024 / disk->get_impl().get_geometry().cylinderSize());
    }


    void
    Partition::Impl::set_region(const Region& region)
    {
	Impl::region = region;

	const PartitionTable* partitiontable = get_partition_table();
	assert(partitiontable);

	const Disk* disk = partitiontable->get_disk();
	assert(disk);

	set_size_k(region.get_length() * disk->get_impl().get_geometry().cylinderSize() / 1024);
    }


    const PartitionTable*
    Partition::Impl::get_partition_table() const
    {
	Devicegraph::Impl::vertex_descriptor v = get_devicegraph()->get_impl().parent(get_vertex());

	return dynamic_cast<const PartitionTable*>(get_devicegraph()->get_impl().graph[v].get());
    }


    void
    Partition::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::SetPartitionId(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Partition::Impl::add_modify_actions(Actiongraph& actiongraph, const Device* lhs_base) const
    {
	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_type() != lhs.get_type())
	{
	    throw runtime_error("cannot change partition type");
	}

	if (get_region().get_start() != lhs.get_region().get_start())
	{
	    throw runtime_error("cannot move partition");
	}

	if (get_id() != lhs.get_id())
	{
	    Action::Base* action = new Action::SetPartitionId(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    void
    Partition::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Partition::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return region == rhs.region && type == rhs.type && id == rhs.id && boot == rhs.boot;
    }


    void
    Partition::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff(log, "region", region, rhs.region);
	storage::log_diff_enum(log, "type", type, rhs.type);
	storage::log_diff_hex(log, "id", id, rhs.id);
	storage::log_diff(log, "boot", boot, rhs.boot);
    }


    void
    Partition::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " region:" << get_region();
    }


    void
    Partition::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	const Devicegraph* g = get_devicegraph();

	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());
	Devicegraph::Impl::vertex_descriptor v2 = g->get_impl().parent(v1);

	const Disk* disk = dynamic_cast<const Disk*>(g->get_impl().graph[v2].get());

	disk->get_impl().process_udev_ids(udev_ids);
    }


    bool
    Partition::Impl::cmp_lt_number(const Partition* lhs, const Partition* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }


    Text
    Partition::Impl::do_create_text(bool doing) const
    {
	return sformat(_("Create partition %1$s (%2$s)"), get_name().c_str(), get_size_string().c_str());
    }


    void
    Partition::Impl::do_create() const
    {
	const PartitionTable* partitiontable = get_partition_table();
	assert(partitiontable);

	const Disk* disk = partitiontable->get_disk();
	assert(disk);

	string cmd_line = PARTEDBIN " -s " + quote(disk->get_name()) + " unit cyl mkpart " +
	    toString(get_type()) + " ";

	if (get_type() != EXTENDED)
	{
	    // TODO look at id
	    cmd_line += "ext2 ";
	}

	cmd_line += decString(get_region().get_start()) + " " + decString(get_region().get_end());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("create partition failed");
    }


    Text
    Partition::Impl::do_set_id_text(bool doing) const
    {
	string tmp = id_to_string(get_id());

	if (tmp.empty())
	    return sformat(_("Set id of partition %1$s to 0x%2$02X"), get_name().c_str(), get_id());
	else
	    return sformat(_("Set id of partition %1$s to %2$s (0x%3$02X)"), get_name().c_str(),
			   tmp.c_str(), get_id());
    }


    void
    Partition::Impl::do_set_id() const
    {
	const PartitionTable* partitiontable = get_partition_table();
	assert(partitiontable);

	const Disk* disk = partitiontable->get_disk();
	assert(disk);

	string cmd_line = PARTEDBIN " -s " + quote(disk->get_name()) + " set " +
	    decString(get_number()) + " type " + decString(get_id());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("set partition id failed");
    }


    Text
    Partition::Impl::do_delete_text(bool doing) const
    {
	return sformat(_("Delete partition %1$s (%2$s)"), get_name().c_str(),
		       get_size_string().c_str());
    }


    void
    Partition::Impl::do_delete() const
    {
	const PartitionTable* partitiontable = get_partition_table();
	assert(partitiontable);

	const Disk* disk = partitiontable->get_disk();
	assert(disk);

	string cmd_line = PARTEDBIN " -s " + disk->get_name() + " rm " + decString(get_number());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("delete partition failed");
    }


    namespace Action
    {

	Text
	SetPartitionId::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Partition* partition = to_partition(device_rhs(actiongraph));
	    return partition->get_impl().do_set_id_text(doing);
	}


	void
	SetPartitionId::commit(const Actiongraph& actiongraph) const
	{
	    const Partition* partition = to_partition(device_rhs(actiongraph));
	    partition->get_impl().do_set_id();
	}

    }

    string
    id_to_string(unsigned int id)
    {
	switch (id)
	{
	    case ID_SWAP: return "Linux Swap";
	    case ID_LINUX: return "Linux";
	    case ID_LVM: return "Linux LVM";
	    case ID_RAID: return "Linux RAID";
	}

	return "";
    }

}



#include <iostream>

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partition>::classname = "Partition";


    Partition::Impl::Impl(const string& name, const Region& region, PartitionType type)
	: BlkDevice::Impl(name, region.to_kb(region.get_length())), region(region), type(type),
	  id(ID_LINUX), boot(false)
    {
    }


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
    Partition::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = systeminfo.getParted(partitionable->get_name());
	Parted::Entry entry;
	if (!parted.getEntry(get_number(), entry))
	    throw;

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
	if (pos == string::npos || pos == get_name().size() - 1)
	    ST_THROW(Exception("partition name has no number"));

	return atoi(get_name().substr(pos + 1).c_str());
    }


    void
    Partition::Impl::set_size_k(unsigned long long size_k)
    {
	BlkDevice::Impl::set_size_k(size_k);

	const Partitionable* partitionable = get_partitionable();

	region.set_length(get_size_k() * 1024 / partitionable->get_impl().get_geometry().cylinderSize());
    }


    void
    Partition::Impl::set_region(const Region& region)
    {
	Impl::region = region;

	const Partitionable* partitionable = get_partitionable();

	const Geometry& geometry = partitionable->get_impl().get_geometry();
	if (region.get_block_size() != geometry.cylinderSize())
	    ST_THROW(DifferentBlockSizes(region.get_block_size(), geometry.cylinderSize()));

	set_size_k(region.get_length() * partitionable->get_impl().get_geometry().cylinderSize() / 1024);
    }


    const PartitionTable*
    Partition::Impl::get_partition_table() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partition_table(get_devicegraph()->get_impl()[vertex]);
    }


    const Partitionable*
    Partition::Impl::get_partitionable() const
    {
	const PartitionTable* partition_table = get_partition_table();

	Devicegraph::Impl::vertex_descriptor vertex =
	    get_devicegraph()->get_impl().parent(partition_table->get_impl().get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[vertex]);
    }


    void
    Partition::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::CreatePartition(get_sid()));
	actions.push_back(new Action::SetPartitionId(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Partition::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
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
    Partition::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
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
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().process_udev_ids(udev_ids);
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
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " -s " + quote(partitionable->get_name()) + " unit cyl mkpart " +
	    toString(get_type()) + " ";

	if (get_type() != EXTENDED)
	{
	    switch (get_id())
	    {
		case ID_SWAP:
		    cmd_line += "linux-swap ";
		    break;

		case ID_DOS16:
		    cmd_line += "fat16 ";
		    break;

		case ID_GPT_BOOT:
		case ID_DOS32:
		    cmd_line += "fat32 ";
		    break;

		case ID_NTFS:
		    cmd_line += "ntfs ";
		    break;

		case ID_APPLE_HFS:
		    cmd_line += "hfs ";
		    break;

		default:
		    cmd_line += "ext2 ";
		    break;
	    }
	}

	cmd_line += to_string(get_region().get_start()) + " " + to_string(get_region().get_end());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create partition failed"));
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
	const Partitionable* partitionable = get_partitionable();

	// TODO

	if (get_id() > 255 || !is_msdos(get_device()))
	    return;

	string cmd_line = PARTEDBIN " -s " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " type " + to_string(get_id());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set partition id failed"));
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
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " -s " + partitionable->get_name() + " rm " + to_string(get_number());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete partition failed"));
    }


    namespace Action
    {

	void
	CreatePartition::add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const
	{
	    Create::add_dependencies(v, actiongraph);

	    // create order of partitions

	    sid_t sid = actiongraph[v]->sid;

	    Devicegraph::Impl::vertex_descriptor v_in_rhs = actiongraph.get_devicegraph(RHS)->get_impl().find_vertex(sid);

	    vector<Devicegraph::Impl::vertex_descriptor> siblings = actiongraph.get_devicegraph(RHS)->get_impl().siblings(v_in_rhs, false);

	    // TODO remember only best candidate in the first place
	    vector<Actiongraph::Impl::vertex_descriptor> w;

	    for (Devicegraph::Impl::vertex_descriptor q : siblings)
	    {
		sid_t s_sid = actiongraph.get_devicegraph(RHS)->get_impl()[q]->get_sid();

		for (Actiongraph::Impl::vertex_descriptor tmp : actiongraph.vertices())
		{
		    sid_t a_sid = actiongraph[tmp]->sid;
		    if (s_sid == a_sid && actiongraph[tmp]->last)
		    {
			const Partition* p_lhs = dynamic_cast<const Partition*>(actiongraph.get_devicegraph(LHS)->get_impl()[q]);
			const Partition* p_rhs = dynamic_cast<const Partition*>(actiongraph.get_devicegraph(RHS)->get_impl()[v_in_rhs]);

			if (p_lhs->get_number() < p_rhs->get_number())
			    w.push_back(tmp);
		    }
		}
	    }

	    if (!w.empty())
	    {
		sort(w.begin(), w.end()); // TODO number sort
		actiongraph.add_edge(w.back(), v);
	    }
	}


	Text
	SetPartitionId::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Partition* partition = to_partition(device_rhs(actiongraph));
	    return partition->get_impl().do_set_id_text(doing);
	}


	void
	SetPartitionId::commit(const Actiongraph::Impl& actiongraph) const
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


    bool
    compare_by_number(const Partition* lhs, const Partition* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    };

}

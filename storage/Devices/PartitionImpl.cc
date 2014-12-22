

#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/Disk.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/StorageDefines.h"


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

	actions.push_back(new Action::CreatePartition(get_sid()));
	actions.push_back(new Action::SetPartitionId(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Partition::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::DeletePartition(get_sid()));

	actiongraph.add_chain(actions);
    }


    namespace Action
    {

	Text
	CreatePartition::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(RHS)->find_device(sid));
	    assert(partition);

	    return sformat(_("Create partition %1$s (%2$s)"), partition->get_name().c_str(),
			   partition->get_size_string().c_str());
	}


	void
	CreatePartition::commit(const Actiongraph& actiongraph) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(RHS)->find_device(sid));
	    assert(partition);

	    const PartitionTable* partitiontable = partition->get_partition_table();
	    assert(partitiontable);

	    const Disk* disk = partitiontable->get_disk();
	    assert(disk);

	    ostringstream cmd_line;

	    cmd_line << PARTEDBIN " -s " << quote(disk->get_name()) << " unit cyl mkpart "
		     << toString(partition->get_type()) << " ";

	    if (partition->get_type() != EXTENDED)
            {
		// TODO look at id
		cmd_line << "ext2 ";
	    }

	    cmd_line << partition->get_region().get_start() << " " << partition->get_region().get_end();

	    cout << cmd_line.str() << endl;

	    SystemCmd cmd(cmd_line.str());
	    if (cmd.retcode() != 0)
		throw runtime_error("create partition failed");
	}


	Text
	SetPartitionId::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(RHS)->find_device(sid));
	    assert(partition);

	    return sformat(_("Set id of partition %1$s to 0x%2$02X"), partition->get_name().c_str(),
			   partition->get_id());
	}


	void
	SetPartitionId::commit(const Actiongraph& actiongraph) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(RHS)->find_device(sid));
	    assert(partition);

	    const PartitionTable* partitiontable = partition->get_partition_table();
	    assert(partitiontable);

	    const Disk* disk = partitiontable->get_disk();
	    assert(disk);

	    ostringstream cmd_line;

	    cmd_line << PARTEDBIN " -s " << quote(disk->get_name()) << " set " << partition->get_number()
		     << " type " << partition->get_id();

	    cout << cmd_line.str() << endl;

	    SystemCmd cmd(cmd_line.str());
	    if (cmd.retcode() != 0)
		throw runtime_error("set partition id failed");
	}


	Text
	DeletePartition::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(LHS)->find_device(sid));
	    assert(partition);

	    return sformat(_("Delete partition %1$s (%2$s)"), partition->get_name().c_str(),
			   partition->get_size_string().c_str());
	}


	void
	DeletePartition::commit(const Actiongraph& actiongraph) const
	{
	    const Partition* partition = to_partition(actiongraph.get_devicegraph(LHS)->find_device(sid));
	    assert(partition);

	    const PartitionTable* partitiontable = partition->get_partition_table();
	    assert(partitiontable);

	    const Disk* disk = partitiontable->get_disk();
	    assert(disk);

	    ostringstream cmd_line;

	    cmd_line << PARTEDBIN " -s " << disk->get_name() << " rm " << partition->get_number();

	    cout << cmd_line.str() << endl;

	    SystemCmd cmd(cmd_line.str());
	    if (cmd.retcode() != 0)
		throw runtime_error("delete partition failed");
	}

    }

}

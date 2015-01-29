

#include <iostream>

#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Devices/Ext4Impl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"


namespace storage
{

    using namespace std;


    Ext4::Impl::Impl(const xmlNode* node)
	: Filesystem::Impl(node)
    {
    }


    void
    Ext4::Impl::save(xmlNode* node) const
    {
	Filesystem::Impl::save(node);
    }


    void
    Ext4::Impl::add_create_actions(Actiongraph& actiongraph) const
    {
	Action::Create* format = new Action::Create(get_sid());
	format->first = true;
	format->last = false;
	Actiongraph::vertex_descriptor v1 = actiongraph.add_vertex(format);

	if (!get_label().empty())
	{
	    Actiongraph::vertex_descriptor tmp = actiongraph.add_vertex(new Action::SetLabel(get_sid()));
	    actiongraph.add_edge(v1, tmp);
	    v1 = tmp;
	}

	if (!get_mountpoints().empty())
	{
	    Actiongraph::vertex_descriptor v2 = actiongraph.add_vertex(new Action::Nop(get_sid(), false, true));

	    for (const string& mountpoint : get_mountpoints())
	    {
		Actiongraph::vertex_descriptor t1 = actiongraph.add_vertex(new Action::Mount(get_sid(), mountpoint));
		Actiongraph::vertex_descriptor t2 = actiongraph.add_vertex(new Action::AddFstab(get_sid(), mountpoint));

		actiongraph.add_edge(v1, t1);
		actiongraph.add_edge(t1, t2);
		actiongraph.add_edge(t2, v2);
	    }
	}
    }


    bool
    Ext4::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Filesystem::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    Ext4::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Filesystem::Impl::log_diff(log, rhs);
    }


    void
    Ext4::Impl::print(std::ostream& out) const
    {
	Filesystem::Impl::print(out);
    }


    void
    Ext4::Impl::do_create() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	blkdevice->get_impl().wait_for_device();

	string cmd_line = MKFSEXT2BIN " -t ext4 -v " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("create ext4 failed");
    }


    void
    Ext4::Impl::do_set_label() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	string cmd_line = TUNE2FSBIN " -L " + quote(get_label()) + " " + quote(blkdevice->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    throw runtime_error("set-label ext4 failed");
    }

}

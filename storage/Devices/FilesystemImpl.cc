

#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "label", label, !label.empty());
	setChildValueIf(node, "uuid", uuid, !uuid.empty());

	for (const string& mountpoint : mountpoints)
	    setChildValue(node, "mountpoint", mountpoint);
    }


    void
    Filesystem::Impl::add_delete_actions(Actiongraph& actiongraph) const
    {
	vector<Action::Base*> actions;

	for (const string& mountpoint : mountpoints)
	{
	    actions.push_back(new Action::RemoveFstab(get_sid(), mountpoint));
	    actions.push_back(new Action::Umount(get_sid(), mountpoint));
	}

	actiongraph.add_chain(actions);
    }


    vector<const BlkDevice*>
    Filesystem::Impl::get_blkdevices() const
    {
	const Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().getDevices<BlkDevice>(devicegraph->get_impl().parents(vertex));
    }


    bool
    Filesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return label == rhs.label && uuid == rhs.uuid && mountpoints == rhs.mountpoints;
    }


    void
    Filesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "label", label, rhs.label);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);
	// storage::log_diff(log, "mountpoints", mountpoints, rhs.mountpoints); // TODO
    }


    void
    Filesystem::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);
    }


    namespace Action
    {

	Text
	Format::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Create %1$s"), device->get_displayname().c_str());
	}


	vector<const BlkDevice*>
	Format::get_blkdevices(const Actiongraph& actiongraph) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);
	    assert(device);

	    const Filesystem* filesystem = dynamic_cast<const Filesystem*>(device);
	    assert(filesystem);

	    vector<const BlkDevice*> blkdevices = filesystem->get_blkdevices();
	    assert(blkdevices.size() >= 1);

	    return blkdevices;
	}


	const BlkDevice*
	Format::get_blkdevice(const Actiongraph& actiongraph) const
	{
	    vector<const BlkDevice*> blkdevices = get_blkdevices(actiongraph);
	    assert(blkdevices.size() == 1);

	    return blkdevices.front();
	}


	Text
	SetLabel::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Set label %1$s"), device->get_displayname().c_str());
	}


	Text
	Mount::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Mount %1$s"), device->get_displayname().c_str());
	}


	Text
	Umount::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(LHS)->find_device(sid);

	    return sformat(_("Unmount %1$s"), device->get_displayname().c_str());
	}


	Text
	AddFstab::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);

	    return sformat(_("Add %1$s to fstab"), device->get_displayname().c_str());
	}


	Text
	RemoveFstab::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const Device* device = actiongraph.get_devicegraph(LHS)->find_device(sid);

	    return sformat(_("Remove %1$s from fstab"), device->get_displayname().c_str());
	}

    }

}



#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"


namespace storage
{

    using namespace std;


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), mount_by(MOUNTBY_DEVICE)
    {
	string tmp;

	getChildValue(node, "label", label);
	getChildValue(node, "uuid", uuid);

	getChildValue(node, "mountpoint", mountpoints);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MOUNTBY_DEVICE);

	if (getChildValue(node, "fstab-options", tmp))
	    fstab_options = splitString(tmp, ",");
    }


    void
    Filesystem::Impl::set_label(const string& label)
    {
	Impl::label = label;
    }


    void
    Filesystem::Impl::add_mountpoint(const string& mountpoint)
    {
	Impl::mountpoints.push_back(mountpoint);
    }


    void
    Filesystem::Impl::set_mount_by(MountByType mount_by)
    {
	Impl::mount_by = mount_by;
    }


    void
    Filesystem::Impl::set_fstab_options(const list<string>& fstab_options)
    {
	Impl::fstab_options = fstab_options;
    }


    void
    Filesystem::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "label", label, !label.empty());
	setChildValueIf(node, "uuid", uuid, !uuid.empty());

	setChildValue(node, "mountpoint", mountpoints);

	setChildValueIf(node, "mount-by", toString(mount_by), mount_by != MOUNTBY_DEVICE);

	if (!fstab_options.empty())
	    setChildValue(node, "fstab-options", boost::join(fstab_options, ","));
    }


    void
    Filesystem::Impl::probe(SystemInfo& systeminfo, EtcFstab& fstab)
    {
	const Devicegraph* g = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());
	const BlkDevice* blkdevice = dynamic_cast<const BlkDevice*>(g->get_impl().graph[v1].get());

	const Blkid& blkid = systeminfo.getBlkid();
	Blkid::Entry entry;
	if (blkid.getEntry(blkdevice->get_name(), entry))
	{
	    label = entry.fs_label;
	    uuid = entry.fs_uuid;
	}

	const ProcMounts& proc_mounts = systeminfo.getProcMounts();
	string mountpoint = proc_mounts.getMount(blkdevice->get_name());
	if (!mountpoint.empty())
	    mountpoints.push_back(mountpoint);

	fstab.setDevice(blkdevice->get_name(), {}, uuid, label, blkdevice->get_udev_ids(),
			blkdevice->get_udev_path());
	FstabEntry fstabentry;
	if (fstab.findDevice(blkdevice->get_name(), fstabentry))
	{
	    mount_by = fstabentry.mount_by;
	    fstab_options = fstabentry.opts;
	}
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

	return label == rhs.label && uuid == rhs.uuid && mountpoints == rhs.mountpoints &&
	    mount_by == rhs.mount_by && fstab_options == rhs.fstab_options;
    }


    void
    Filesystem::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "label", label, rhs.label);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);

	storage::log_diff(log, "mountpoints", mountpoints, rhs.mountpoints);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "fstab-options", fstab_options, rhs.fstab_options);
    }


    void
    Filesystem::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	if (!label.empty())
	    out << " label:" << label;

	if (!uuid.empty())
	    out << " uuid:" << uuid;

	if (!mountpoints.empty())
	    out << " mountpoints:" << mountpoints;

	if (!fstab_options.empty())
	    out << " fstab-options:" << fstab_options;
    }


    namespace Action
    {

	Text
	Format::text(const Actiongraph& actiongraph, bool doing) const
	{
	    const BlkDevice* blkdevice = get_blkdevice(actiongraph);

	    const Device* device = actiongraph.get_devicegraph(RHS)->find_device(sid);
	    assert(is_filesystem(device));

	    return sformat(_("Create %1$s on %2$s (%3$s)"), device->get_displayname().c_str(),
			   blkdevice->get_name().c_str(), blkdevice->get_size_string().c_str());
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

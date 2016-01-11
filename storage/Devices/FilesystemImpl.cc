

#include <iostream>

#include "storage/Devices/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Filesystem>::classname = "Filesystem";


    Filesystem::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), label(), uuid(), mountpoints({}), mount_by(MOUNTBY_DEVICE),
	  fstab_options({}), mkfs_options(), tune_options()
    {
	string tmp;

	getChildValue(node, "label", label);
	getChildValue(node, "uuid", uuid);

	getChildValue(node, "mountpoint", mountpoints);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MOUNTBY_DEVICE);

	if (getChildValue(node, "fstab-options", tmp))
	    fstab_options = splitString(tmp, ",");

	getChildValue(node, "mkfs-options", mkfs_options);
	getChildValue(node, "tune-options", tune_options);
    }


    void
    Filesystem::Impl::set_label(const string& label)
    {
	Impl::label = label;
    }


    void
    Filesystem::Impl::set_mountpoints(const vector<string>& mountpoints)
    {
	Impl::mountpoints = mountpoints;
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
    Filesystem::Impl::set_mkfs_options(const string& mkfs_options)
    {
	Impl::mkfs_options = mkfs_options;
    }


    void
    Filesystem::Impl::set_tune_options(const string& tune_options)
    {
	Impl::tune_options = tune_options;
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

	setChildValueIf(node, "mkfs-options", mkfs_options, !mkfs_options.empty());
	setChildValueIf(node, "tune-options", tune_options, !tune_options.empty());
    }


    void
    Filesystem::Impl::probe_pass_3(Devicegraph* probed, SystemInfo& systeminfo, EtcFstab& fstab)
    {
	const Devicegraph* g = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor v1 = g->get_impl().parent(get_vertex());
	const BlkDevice* blkdevice = dynamic_cast<const BlkDevice*>(g->get_impl()[v1]);

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
    Filesystem::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	Action::Base* first = nullptr;
	Action::Base* last = nullptr;

	Action::Create* format = new Action::Create(get_sid());
	Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(format);
	first = last = format;

	if (!get_label().empty())
	{
	    Action::SetLabel* set_label = new Action::SetLabel(get_sid());
	    Actiongraph::Impl::vertex_descriptor tmp = actiongraph.add_vertex(set_label);
	    actiongraph.add_edge(v1, tmp);
	    v1 = tmp;

	    last = set_label;
	}

	if (!get_mountpoints().empty())
	{
	    Action::Base* sync = new Action::Create(get_sid(), true);
	    Actiongraph::Impl::vertex_descriptor v2 = actiongraph.add_vertex(sync);
	    last = sync;

	    for (const string& mountpoint : get_mountpoints())
	    {
		Action::Mount* mount = new Action::Mount(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t1 = actiongraph.add_vertex(mount);

		Action::AddFstab* add_fstab = new Action::AddFstab(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t2 = actiongraph.add_vertex(add_fstab);

		actiongraph.add_edge(v1, t1);
		actiongraph.add_edge(t1, t2);
		actiongraph.add_edge(t2, v2);
	    }
	}

	first->first = true;
	last->last = true;
    }


    void
    Filesystem::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	Action::Base* first = nullptr;
	Action::Base* last = nullptr;

	Action::Base* sync1 = new Action::Delete(get_sid(), true);
	Actiongraph::Impl::vertex_descriptor v1 = actiongraph.add_vertex(sync1);
	first = last = sync1;

	if (!get_mountpoints().empty())
	{
	    Action::Base* sync2 = new Action::Delete(get_sid(), true);
	    Actiongraph::Impl::vertex_descriptor v2 = actiongraph.add_vertex(sync2);
	    first = sync2;

	    for (const string& mountpoint : get_mountpoints())
	    {
		Action::RemoveFstab* remove_fstab = new Action::RemoveFstab(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t1 = actiongraph.add_vertex(remove_fstab);

		Action::Umount* umount = new Action::Umount(get_sid(), mountpoint);
		Actiongraph::Impl::vertex_descriptor t2 = actiongraph.add_vertex(umount);

		actiongraph.add_edge(v2, t1);
		actiongraph.add_edge(t1, t2);
		actiongraph.add_edge(t2, v1);
	    }
	}

	first->first = true;
	last->last = true;
    }


    vector<const BlkDevice*>
    Filesystem::Impl::get_blkdevices() const
    {
	const Devicegraph* devicegraph = get_devicegraph();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph->get_impl().filter_devices_of_type<BlkDevice>(devicegraph->get_impl().parents(vertex));
    }


    const BlkDevice*
    Filesystem::Impl::get_blkdevice() const
    {
	vector<const BlkDevice*> blkdevices = get_blkdevices();
	assert(blkdevices.size() == 1);

	return blkdevices.front();
    }


    bool
    Filesystem::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return label == rhs.label && uuid == rhs.uuid && mountpoints == rhs.mountpoints &&
	    mount_by == rhs.mount_by && fstab_options == rhs.fstab_options &&
	    mkfs_options == rhs.mkfs_options && tune_options == rhs.tune_options;
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

	storage::log_diff(log, "mkfs-options", mkfs_options, rhs.mkfs_options);
	storage::log_diff(log, "tune-options", tune_options, rhs.tune_options);
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

	if (!mkfs_options.empty())
	    out << " mkfs-options:" << mkfs_options;

	if (!tune_options.empty())
	    out << " tune-options:" << tune_options;
    }


    string
    Filesystem::Impl::get_mount_by_string() const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	string ret = blkdevice->get_name();

	switch (mount_by)
	{
	    case MOUNTBY_UUID:
		if (!uuid.empty())
		    ret = "UUID=" + uuid;
		else
		    y2err("no uuid defined");
		break;

	    case MOUNTBY_LABEL:
		if (!label.empty())
		    ret = "LABEL=" + label;
		else
		    y2err("no label defined");
		break;

	    case MOUNTBY_ID:
		if (!blkdevice->get_udev_ids().empty())
		    ret = DEVDIR "/disk/by-id/" + blkdevice->get_udev_ids().front();
		else
		    y2err("no udev-id defined");
		break;

	    case MOUNTBY_PATH:
		if (!blkdevice->get_udev_path().empty())
		    ret = DEVDIR "/disk/by-path/" + blkdevice->get_udev_path();
		else
		    y2err("no udev-path defined");
		break;

	    case MOUNTBY_DEVICE:
		break;
	}

	return ret;
    }


    Text
    Filesystem::Impl::do_create_text(bool doing) const
    {
	// TODO handle multiple blkdevices

	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Create %1$s on %2$s (%3$s)"), get_displayname().c_str(),
		       blkdevice->get_name().c_str(), blkdevice->get_size_string().c_str());
    }


    Text
    Filesystem::Impl::do_set_label_text(bool doing) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Set label of %1$s to %2$s"), blkdevice->get_name().c_str(),
		       label.c_str());
    }


    void
    Filesystem::Impl::do_set_label() const
    {
	// TODO - stub
    }


    Text
    Filesystem::Impl::do_mount_text(const string& mountpoint, bool doing) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Mount %1$s at %2$s"), blkdevice->get_name().c_str(),
		       mountpoint.c_str());
    }


    void
    Filesystem::Impl::do_mount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);
	if (access(real_mountpoint.c_str(), R_OK ) != 0)
	{
	    createPath(real_mountpoint);
	}

	string cmd_line = MOUNTBIN " -t " + toString(get_type()) + " " + quote(blkdevice->get_name())
	    + " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("mount failed"));
    }


    Text
    Filesystem::Impl::do_umount_text(const string& mountpoint, bool doing) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Unmount %1$s at %2$s"), blkdevice->get_name().c_str(),
		       mountpoint.c_str());
    }


    void
    Filesystem::Impl::do_umount(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	string real_mountpoint = storage.get_impl().prepend_rootprefix(mountpoint);

	string cmd_line = UMOUNTBIN " " + quote(real_mountpoint);
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("umount failed"));
    }


    Text
    Filesystem::Impl::do_add_fstab_text(const string& mountpoint, bool doing) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Add mountpoint %1$s of %2$s to fstab"), mountpoint.c_str(),
		       blkdevice->get_name().c_str());
    }


    void
    Filesystem::Impl::do_add_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	const BlkDevice* blkdevice = get_blkdevice();

	// TODO

	FstabChange entry;
	entry.device = blkdevice->get_name();
	entry.dentry = get_mount_by_string();
	entry.mount = mountpoint;
	entry.fs = toString(get_type());
	entry.opts = fstab_options;

	fstab.addEntry(entry);
	fstab.flush();
    }


    Text
    Filesystem::Impl::do_remove_fstab_text(const string& mountpoint, bool doing) const
    {
	const BlkDevice* blkdevice = get_blkdevice();

	return sformat(_("Remove mountpoint %1$s of %2$s from fstab"), mountpoint.c_str(),
		       blkdevice->get_name().c_str());
    }


    void
    Filesystem::Impl::do_remove_fstab(const Actiongraph::Impl& actiongraph, const string& mountpoint) const
    {
	const Storage& storage = actiongraph.get_storage();

	EtcFstab fstab(storage.get_impl().prepend_rootprefix("/etc"));	// TODO pass as parameter

	const BlkDevice* blkdevice = get_blkdevice();

	// TODO error handling

	FstabKey entry(blkdevice->get_name(), mountpoint);
	fstab.removeEntry(entry);
	fstab.flush();
    }


    namespace Action
    {

	Text
	SetLabel::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    return filesystem->get_impl().do_set_label_text(doing);
	}


	void
	SetLabel::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    filesystem->get_impl().do_set_label();
	}


	Text
	Mount::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    return filesystem->get_impl().do_mount_text(mountpoint, doing);
	}


	void
	Mount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    filesystem->get_impl().do_mount(actiongraph, mountpoint);
	}


	Text
	Umount::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Filesystem* filesystem = to_filesystem(device_lhs(actiongraph));
	    return filesystem->get_impl().do_umount_text(mountpoint, doing);
	}


	void
	Umount::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(device_lhs(actiongraph));
	    filesystem->get_impl().do_umount(actiongraph, mountpoint);
	}


	Text
	AddFstab::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    return filesystem->get_impl().do_add_fstab_text(mountpoint, doing);
	}


	void
	AddFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(device_rhs(actiongraph));
	    filesystem->get_impl().do_add_fstab(actiongraph, mountpoint);
	}


	void
	AddFstab::add_dependencies(Actiongraph::Impl::vertex_descriptor v, Actiongraph::Impl& actiongraph) const
	{
	    if (mountpoint == "swap")
		if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		    actiongraph.add_edge(*actiongraph.mount_root_filesystem, v);
	}


	Text
	RemoveFstab::text(const Actiongraph::Impl& actiongraph, bool doing) const
	{
	    const Filesystem* filesystem = to_filesystem(device_lhs(actiongraph));
	    return filesystem->get_impl().do_remove_fstab_text(mountpoint, doing);
	}


	void
	RemoveFstab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Filesystem* filesystem = to_filesystem(device_lhs(actiongraph));
	    filesystem->get_impl().do_remove_fstab(actiongraph, mountpoint);
	}

    }

}

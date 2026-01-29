/*
 * Copyright (c) [2017-2025] SUSE LLC
 *
 * All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, contact Novell, Inc.
 *
 * To contact Novell about this file by physical or electronic mail, you may
 * find current contact information at www.novell.com.
 */


#include "storage/Utils/XmlFile.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Holders/FilesystemUserImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/EtcFstab.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/StorageImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Redirect.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/MountImpl.h"
#include "storage/Actions/UnmountImpl.h"
#include "storage/Actions/AddToEtcFstabImpl.h"
#include "storage/Actions/UpdateInEtcFstabImpl.h"
#include "storage/Actions/RenameInEtcFstabImpl.h"
#include "storage/Actions/RemoveFromEtcFstabImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<MountPoint>::classname = "MountPoint";


    MountPoint::Impl::Impl(const string& path)
	: Device::Impl()
    {
#if 0
	if (!valid_path(path))
	    ST_THROW(InvalidMountPointPath(path));
#endif
	set_path(path);
    }


    MountPoint::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
	string tmp;

	if (getChildValue(node, "path", tmp))
	    set_path(tmp);

	getChildValue(node, "rootprefixed", rootprefixed);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "mount-options", tmp))
	    mount_options.parse(tmp);

	if (getChildValue(node, "mount-type", tmp))
	    mount_type = toValueWithFallback(tmp, FsType::UNKNOWN);

	getChildValue(node, "active", active);
	getChildValue(node, "in-etc-fstab", in_etc_fstab);
	getChildValue(node, "id-used-in-etc-fstab", fstab_anchor.id);

	getChildValue(node, "freq", freq);
	getChildValue(node, "passno", passno);
    }


    string
    MountPoint::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Mount Point").translated;
    }


    void
    MountPoint::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "path", path);
	setChildValue(node, "rootprefixed", rootprefixed);

	setChildValue(node, "mount-by", toString(mount_by));

	if (!mount_options.empty())
	    setChildValue(node, "mount-options", mount_options.format());

	setChildValue(node, "mount-type", toString(mount_type));
	setChildValue(node, "active", active);
	setChildValue(node, "in-etc-fstab", in_etc_fstab);
	setChildValueIf(node, "id-used-in-etc-fstab", fstab_anchor.id, fstab_anchor.id);

	setChildValue(node, "freq", freq);
	setChildValue(node, "passno", passno);
    }


    void
    MountPoint::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	if (!has_single_parent_of_type<const Mountable>())
	    ST_THROW(Exception("Mountable parent missing"));

#if 0
	if (!valid_path(path))
	    ST_THROW(InvalidMountPointPath(path));
#endif
    }


    void
    MountPoint::Impl::set_path(const string& path)
    {
#if 0
	if (!valid_path(path))
	    ST_THROW(InvalidMountPointPath(path));
#endif

	Impl::path = normalize_path(path);
    }


    string
    MountPoint::Impl::get_displayname() const
    {
	return path;
    }


    bool
    MountPoint::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return path == rhs.path && mount_by == rhs.mount_by && mount_type == rhs.mount_type &&
	    mount_options == rhs.mount_options && freq == rhs.freq && passno == rhs.passno &&
	    active == rhs.active && in_etc_fstab == rhs.in_etc_fstab && rootprefixed == rhs.rootprefixed;
    }


    void
    MountPoint::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "path", path, rhs.path);
	storage::log_diff(log, "rootprefixed", rootprefixed, rhs.rootprefixed);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff_enum(log, "mount-type", mount_type, rhs.mount_type);

	storage::log_diff(log, "mount-options", mount_options.get_opts(), rhs.mount_options.get_opts());

	storage::log_diff(log, "freq", freq, rhs.freq);
	storage::log_diff(log, "passno", passno, rhs.passno);

	storage::log_diff(log, "active", active, rhs.active);
	storage::log_diff(log, "in-etc-fstab", in_etc_fstab, rhs.in_etc_fstab);
    }


    void
    MountPoint::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " path:" << path;

	if (rootprefixed)
	    out << " rootprefixed";

	out << " mount-by:" << toString(mount_by);

	out << " mount-type:" << toString(mount_type);

	out << " mount-options:" << mount_options.get_opts();

	out << " freq:" << freq << " passno:" << passno;

	if (active)
	    out << " active";

	if (in_etc_fstab)
	    out << " in-etc-fstab";
    }


    ResizeInfo
    MountPoint::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    bool
    MountPoint::Impl::has_mountable() const
    {
	return has_single_parent_of_type<const Mountable>();
    }


    Mountable*
    MountPoint::Impl::get_mountable()
    {
	return get_single_parent_of_type<Mountable>();
    }


    const Mountable*
    MountPoint::Impl::get_mountable() const
    {
	return get_single_parent_of_type<const Mountable>();
    }


    Filesystem*
    MountPoint::Impl::get_filesystem()
    {
	return get_mountable()->get_filesystem();
    }


    const Filesystem*
    MountPoint::Impl::get_filesystem() const
    {
	return get_mountable()->get_filesystem();
    }


    string
    MountPoint::Impl::get_mount_name() const
    {
	return get_mountable()->get_impl().get_mount_name();
    }


    string
    MountPoint::Impl::get_mount_by_name() const
    {
	return get_mountable()->get_impl().get_mount_by_name(get_non_impl());
    }


    void
    MountPoint::Impl::set_mount_type(FsType mount_type)
    {
	if (mount_type == FsType::UNKNOWN)
	    ST_THROW(Exception("illegal mount type"));

	Impl::mount_type = mount_type;
    }


    void
    MountPoint::Impl::set_default_mount_type()
    {
	set_mount_type(get_mountable()->get_impl().get_default_mount_type());
    }


    string
    MountPoint::Impl::get_rootprefixed_path() const
    {
	return MountPointPath(path, rootprefixed).fullpath(get_storage()->get_rootprefix());
    }


    uf_t
    MountPoint::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	uf_t ret = Device::Impl::used_features(used_features_dependency_type);

	if (active || in_etc_fstab)
	    ret |= get_mountable()->get_impl().used_features_pure();

	ret |= get_mountable()->get_impl().used_features_pure(get_non_impl());

	return ret;
    }


    vector<MountByType>
    MountPoint::Impl::possible_mount_bys() const
    {
	vector<MountByType> ret;

	const Filesystem* filesystem = get_filesystem();

	for (MountByType mount_by_type : filesystem->get_impl().possible_mount_bys())
	    ret.push_back(mount_by_type);

	if (is_blk_filesystem(filesystem))
	{
	    const BlkFilesystem* blk_filesystem = to_blk_filesystem(filesystem);
	    vector<const BlkDevice*> blk_devices = blk_filesystem->get_blk_devices();

	    // TODO handle several devices

	    if (!blk_devices.empty())
		for (MountByType mount_by_type : blk_devices[0]->get_impl().possible_mount_bys())
		    ret.push_back(mount_by_type);
	}

	return ret;
    }


    void
    MountPoint::Impl::set_default_mount_by()
    {
	set_mount_by(get_mountable()->get_impl().get_default_mount_by());
    }


    void
    MountPoint::Impl::set_mount_options(const MountOpts& mount_options)
    {
	Impl::mount_options = mount_options;
    }


    void
    MountPoint::Impl::set_mount_options(const vector<string>& mount_options)
    {
	Impl::mount_options.set_opts(mount_options);
    }


    MountOpts
    MountPoint::Impl::default_mount_options() const
    {
	return get_mountable()->get_impl().default_mount_options();
    }


    void
    MountPoint::Impl::set_default_mount_options()
    {
	set_mount_options(default_mount_options());
    }


    bool
    MountPoint::Impl::is_read_only() const
    {
	return mount_options.contains("ro");
    }


    void
    MountPoint::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	if (active)
	    actions.push_back(make_shared<Action::Mount>(get_sid()));

	if (in_etc_fstab)
	    actions.push_back(make_shared<Action::AddToEtcFstab>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    MountPoint::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	vector<shared_ptr<Action::Base>> actions;

	if (lhs.in_etc_fstab && !in_etc_fstab)
	{
	    actions.push_back(make_shared<Action::RemoveFromEtcFstab>(get_sid()));
	}

	// A unmount action could be required when the device is set as unmounted in the target system
	// or when some of its mount properties have changed (e.g., path, mount type). But the unmount
	// action only makes sense if the device is currently mounted in the system.
	if (lhs.active && (!active || lhs.path != path || lhs.rootprefixed != rootprefixed ||
			   lhs.mount_type != mount_type))
	{
	    actions.push_back(make_shared<Action::Unmount>(get_sid()));
	}

	if (lhs.in_etc_fstab && in_etc_fstab)
	{
	    if (lhs.mount_by != mount_by || lhs.mount_options != mount_options ||
		lhs.freq != freq || lhs.passno != passno || lhs.path != path ||
		lhs.rootprefixed != rootprefixed || lhs.mount_type != mount_type)
	    {
		actions.push_back(make_shared<Action::UpdateInEtcFstab>(get_sid()));
	    }
	}

	// A mount action could be required when the device is currently unmounted in the system or when
	// when some of its mount properties have changed (e.g., path, mount type). But the mount action
	// only makes sense if the device is set as mounted in the target system.
	if (active && (!lhs.active || lhs.path != path || lhs.rootprefixed != rootprefixed ||
		       lhs.mount_type != mount_type))
	{
	    actions.push_back(make_shared<Action::Mount>(get_sid()));
	}

	if (!lhs.in_etc_fstab && in_etc_fstab)
	{
	    actions.push_back(make_shared<Action::AddToEtcFstab>(get_sid()));
	}

	// Check whether the name of the device used in fstab has changed.

	if (lhs.is_in_etc_fstab() && is_in_etc_fstab() && mount_by_references_blk_device(get_mount_by()))
	{
	    const Filesystem* filesystem = get_filesystem();
	    if (is_blk_filesystem(filesystem))
	    {
		const BlkFilesystem* blk_filesystem = to_blk_filesystem(filesystem);
		const BlkDevice* blk_device = blk_filesystem->get_impl().get_etc_fstab_blk_device(get_non_impl());

		const Devicegraph* devicegraph_lhs = actiongraph.get_devicegraph(LHS);

		if (devicegraph_lhs->device_exists(blk_device->get_sid()))
		{
		    const BlkDevice* blk_device_lhs = to_blk_device(devicegraph_lhs->find_device(blk_device->get_sid()));

		    if (blk_device->get_name() != blk_device_lhs->get_name())
		    {
			actions.push_back(make_shared<Action::RenameInEtcFstab>(get_sid(), blk_device_lhs));
		    }
		}
	    }
	}

	actiongraph.add_chain(actions);
    }


    void
    MountPoint::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	if (in_etc_fstab)
	    actions.push_back(make_shared<Action::RemoveFromEtcFstab>(get_sid()));

	if (active)
	    actions.push_back(make_shared<Action::Unmount>(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    MountPoint::Impl::do_mount_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_mount_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_mount(CommitData& commit_data, const CommitOptions& commit_options)
    {
	get_mountable()->get_impl().do_mount(commit_data, commit_options, get_non_impl());
    }


    uf_t
    MountPoint::Impl::do_mount_used_features() const
    {
	return get_mountable()->get_impl().do_mount_used_features();
    }


    Text
    MountPoint::Impl::do_unmount_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_unmount_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_unmount(CommitData& commit_data)
    {
	get_mountable()->get_impl().do_unmount(commit_data, get_non_impl());
    }


    uf_t
    MountPoint::Impl::do_unmount_used_features() const
    {
	return get_mountable()->get_impl().do_unmount_used_features();
    }


    Text
    MountPoint::Impl::do_add_to_etc_fstab_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_add_to_etc_fstab_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_add_to_etc_fstab(CommitData& commit_data) const
    {
	get_mountable()->get_impl().do_add_to_etc_fstab(commit_data, get_non_impl());
    }


    Text
    MountPoint::Impl::do_update_in_etc_fstab_text(const Device* lhs, Tense tense) const
    {
	return get_mountable()->get_impl().do_update_in_etc_fstab_text(get_non_impl(), lhs, tense);
    }


    void
    MountPoint::Impl::do_update_in_etc_fstab(CommitData& commit_data, const Device* lhs) const
    {
	get_mountable()->get_impl().do_update_in_etc_fstab(commit_data, lhs, get_non_impl());
    }


    Text
    MountPoint::Impl::do_remove_from_etc_fstab_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_remove_from_etc_fstab_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_remove_from_etc_fstab(CommitData& commit_data) const
    {
	get_mountable()->get_impl().do_remove_from_etc_fstab(commit_data, get_non_impl());
    }


    Text
    MountPoint::Impl::do_rename_in_etc_fstab_text(const CommitData& commit_data,
						  const Action::RenameInEtcFstab* action) const
    {
	const BlkDevice* renamed_blk_device_lhs = action->get_renamed_blk_device(commit_data.actiongraph, LHS);
	const BlkDevice* renamed_blk_device_rhs = action->get_renamed_blk_device(commit_data.actiongraph, RHS);

	Text text = tenser(commit_data.tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda6),
			   // %3$s is replaced by device name (e.g. /dev/sda5)
			   _("Rename mount point %1$s from %2$s to %3$s in /etc/fstab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by mount point (e.g. /home),
			   // %2$s is replaced by device name (e.g. /dev/sda6),
			   // %3$s is replaced by device name (e.g. /dev/sda5)
			   _("Renaming mount point %1$s from %2$s to %3$s in /etc/fstab"));

	return sformat(text, get_path(), renamed_blk_device_lhs->get_name(),
		       renamed_blk_device_rhs->get_name());
    }


    void
    MountPoint::Impl::do_rename_in_etc_fstab(CommitData& commit_data, const Action::RenameInEtcFstab* action) const
    {
	const Mountable* mountable = get_mountable();

	EtcFstab& etc_fstab = commit_data.get_etc_fstab();

	const FstabAnchor& fstab_anchor = get_fstab_anchor();

	for (FstabEntry* entry : mountable->get_impl().find_etc_fstab_entries(etc_fstab, fstab_anchor))
	{
	    entry->set_spec(get_mount_by_name());
	    etc_fstab.log_diff();
	    etc_fstab.write();
	}
    }


    void
    MountPoint::Impl::immediate_activate()
    {
	const MountPoint* tmp_mount_point = redirect_to_system(get_non_impl());

	tmp_mount_point->get_mountable()->get_impl().immediate_activate(get_non_impl());
    }


    void
    MountPoint::Impl::immediate_deactivate()
    {
	const MountPoint* tmp_mount_point = redirect_to_system(get_non_impl());

	tmp_mount_point->get_mountable()->get_impl().immediate_deactivate(get_non_impl());
    }

}

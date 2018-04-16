/*
 * Copyright (c) [2017-2018] SUSE LLC
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


#include <boost/algorithm/string.hpp>

#include "storage/Utils/XmlFile.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/Filesystems/MountPointImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/HumanString.h"
#include "storage/EtcFstab.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/StorageImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Redirect.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<MountPoint>::classname = "MountPoint";


    MountPoint::Impl::Impl(const string& path)
	: Device::Impl(), path(path), mount_by(MountByType::DEVICE), mount_type(FsType::UNKNOWN),
	  freq(0), passno(0), active(true), in_etc_fstab(true)
    {
#if 0
	if (!valid_path(path))
	    ST_THROW(InvalidMountPointPath(path));
#endif
    }


    MountPoint::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), path(), mount_by(MountByType::DEVICE), mount_type(FsType::UNKNOWN),
	  freq(0), passno(0), active(true), in_etc_fstab(true)
    {
	string tmp;

	getChildValue(node, "path", path);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "mount-options", tmp))
	    mount_options.parse(tmp);

	if (getChildValue(node, "mount-type", tmp))
	    mount_type = toValueWithFallback(tmp, FsType::UNKNOWN);

	getChildValue(node, "active", active);
	getChildValue(node, "in-etc-fstab", in_etc_fstab);

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

	setChildValue(node, "mount-by", toString(mount_by));

	if (!mount_options.empty())
	    setChildValue(node, "mount-options", mount_options.format());

	setChildValue(node, "mount-type", toString(mount_type));
	setChildValue(node, "active", active);
	setChildValue(node, "in-etc-fstab", in_etc_fstab);

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

	Impl::path = path;
    }


    bool
    MountPoint::Impl::valid_path(const string& path)
    {
	return path == "swap" || boost::starts_with(path, "/");
    }


    string
    MountPoint::Impl::normalize_path(const string& path)
    {
	return path;		// TODO
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
	    active == rhs.active && in_etc_fstab == rhs.in_etc_fstab;
    }


    void
    MountPoint::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "path", path, rhs.path);

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
    MountPoint::Impl::detect_resize_info() const
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
	return get_mountable()->get_impl().get_mount_by_name(get_mount_by());
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


    void
    MountPoint::Impl::set_default_mount_options()
    {
	set_mount_options(get_mountable()->get_impl().get_default_mount_options());
    }


    void
    MountPoint::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (active)
	    actions.push_back(new Action::Mount(get_sid()));

	if (in_etc_fstab)
	    actions.push_back(new Action::AddToEtcFstab(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    MountPoint::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	vector<Action::Base*> actions;

	if (lhs.in_etc_fstab && !in_etc_fstab)
	{
	    actions.push_back(new Action::RemoveFromEtcFstab(get_sid()));
	}

	if ((lhs.active && !active) || (lhs.path != path))
	{
	    actions.push_back(new Action::Unmount(get_sid()));
	}

	if (lhs.in_etc_fstab && in_etc_fstab)
	{
	    if (lhs.mount_by != mount_by || lhs.mount_options != mount_options ||
		lhs.freq != freq || lhs.passno != passno || lhs.path != path)
	    {
		actions.push_back(new Action::UpdateInEtcFstab(get_sid()));
	    }
	}

	if ((!lhs.active && active) || (lhs.path != path))
	{
	    actions.push_back(new Action::Mount(get_sid()));
	}

	if (!lhs.in_etc_fstab && in_etc_fstab)
	{
	    actions.push_back(new Action::AddToEtcFstab(get_sid()));
	}

	actiongraph.add_chain(actions);
    }


    void
    MountPoint::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (in_etc_fstab)
	    actions.push_back(new Action::RemoveFromEtcFstab(get_sid()));

	if (active)
	    actions.push_back(new Action::Unmount(get_sid()));

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


    namespace Action
    {

	Text
	Mount::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    return mount_point->get_impl().do_mount_text(commit_data.tense);
	}


	void
	Mount::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    mount_point->get_impl().do_mount(commit_data, commit_options);
	}


	const string&
	Mount::get_path(Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph));
	    return mount_point->get_path();
	}


	const MountPoint*
	Mount::get_mount_point(Actiongraph::Impl& actiongraph) const
	{
	    return to_mount_point(get_device(actiongraph));
	}


	Text
	Unmount::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    return mount_point->get_impl().do_unmount_text(commit_data.tense);
	}


	void
	Unmount::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    mount_point->get_impl().do_unmount(commit_data);
	}


	Text
	AddToEtcFstab::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    return mount_point->get_impl().do_add_to_etc_fstab_text(commit_data.tense);
	}


	void
	AddToEtcFstab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    mount_point->get_impl().do_add_to_etc_fstab(commit_data);
	}


	const string&
	AddToEtcFstab::get_path(Actiongraph::Impl& actiongraph) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(actiongraph, RHS));
	    return mount_point->get_path();
	}


	void
	AddToEtcFstab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					Actiongraph::Impl& actiongraph) const
	{
	    if (get_path(actiongraph) == "swap")
		if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		    actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}


	Text
	UpdateInEtcFstab::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point_lhs = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    const MountPoint* mount_point_rhs = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    return mount_point_rhs->get_impl().do_update_in_etc_fstab_text(mount_point_lhs, commit_data.tense);
	}


	void
	UpdateInEtcFstab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point_lhs = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    const MountPoint* mount_point_rhs = to_mount_point(get_device(commit_data.actiongraph, RHS));
	    mount_point_rhs->get_impl().do_update_in_etc_fstab(commit_data, mount_point_lhs);
	}


	Text
	RemoveFromEtcFstab::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    return mount_point->get_impl().do_remove_from_etc_fstab_text(commit_data.tense);
	}


	void
	RemoveFromEtcFstab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph, LHS));
	    mount_point->get_impl().do_remove_from_etc_fstab(commit_data);
	}

    }

}

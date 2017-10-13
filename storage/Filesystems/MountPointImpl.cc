/*
 * Copyright (c) 2017 SUSE LLC
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


#include <iostream>
#include <boost/algorithm/string.hpp>

#include "storage/Utils/XmlFile.h"
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


namespace storage
{

    using namespace std;


    const char* DeviceTraits<MountPoint>::classname = "MountPoint";


    MountPoint::Impl::Impl(const string& path)
	: Device::Impl(), path(path), mount_by(MountByType::DEVICE), freq(0), passno(0),
	  active(true), in_etc_fstab(true)
    {
    }


    MountPoint::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), path(), mount_by(MountByType::DEVICE), freq(0), passno(0),
	  active(true), in_etc_fstab(true)
    {
	string tmp;

	getChildValue(node, "path", path);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "mount-options", tmp))
	    mount_options.parse(tmp);

	getChildValue(node, "in-etc-fstab", in_etc_fstab);

	getChildValue(node, "freq", freq);
	getChildValue(node, "passno", passno);
    }


    void
    MountPoint::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValue(node, "path", path);

	setChildValue(node, "mount-by", toString(mount_by));

	if (!mount_options.empty())
	    setChildValue(node, "mount-options", mount_options.format());

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

	return path == rhs.path && mount_by == rhs.mount_by && mount_options == rhs.mount_options &&
	    freq == rhs.freq && passno == rhs.passno && active ==  rhs.active &&
	    in_etc_fstab == rhs.in_etc_fstab;
    }


    void
    MountPoint::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "path", path, rhs.path);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

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

	out << " mount-options:" << mount_options.get_opts();

	out << " freq:" << freq << " passno:" << passno;

	if (active)
	    out << " active";

	if (in_etc_fstab)
	    out << " in-etc-fstab";
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


    FsType
    MountPoint::Impl::get_mount_type() const
    {
	return get_mountable()->get_impl().get_mount_type();
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

	if (!lhs.active && active)
	{
	    Action::Base* action = new Action::Mount(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.active && !active)
	{
	    Action::Base* action = new Action::Umount(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (!lhs.in_etc_fstab && in_etc_fstab)
	{
	    Action::Base* action = new Action::AddToEtcFstab(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.in_etc_fstab && !in_etc_fstab)
	{
	    Action::Base* action = new Action::RemoveFromEtcFstab(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.in_etc_fstab && in_etc_fstab)
	{
	    if (lhs.mount_by != mount_by || lhs.mount_options != mount_options ||
		lhs.freq != freq || lhs.passno != passno)
	    {
		Action::Base* action = new Action::UpdateInEtcFstab(get_sid());
		actiongraph.add_vertex(action);
	    }
	}
    }


    void
    MountPoint::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (in_etc_fstab)
	    actions.push_back(new Action::RemoveFromEtcFstab(get_sid()));

	if (active)
	    actions.push_back(new Action::Umount(get_sid()));

	actiongraph.add_chain(actions);
    }


    Text
    MountPoint::Impl::do_mount_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_mount_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_mount(CommitData& commit_data, const CommitOptions& commit_options) const
    {
	get_mountable()->get_impl().do_mount(commit_data, commit_options, get_non_impl());
    }


    Text
    MountPoint::Impl::do_umount_text(Tense tense) const
    {
	return get_mountable()->get_impl().do_umount_text(get_non_impl(), tense);
    }


    void
    MountPoint::Impl::do_umount(CommitData& commit_data) const
    {
	get_mountable()->get_impl().do_umount(commit_data, get_non_impl());
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
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
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
	Umount::text(const CommitData& commit_data) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    return mount_point->get_impl().do_umount_text(commit_data.tense);
	}


	void
	Umount::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const MountPoint* mount_point = to_mount_point(get_device(commit_data.actiongraph));
	    mount_point->get_impl().do_umount(commit_data);
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

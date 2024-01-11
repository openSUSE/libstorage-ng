/*
 * Copyright (c) [2020-2023] SUSE LLC
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


#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/DevicegraphImpl.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/Format.h"
#include "storage/UsedFeatures.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Holders/BtrfsQgroupRelation.h"
#include "storage/Storage.h"
#include "storage/Redirect.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/DeleteImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<BtrfsQgroup>::classname = "BtrfsQgroup";

    const BtrfsQgroup::id_t BtrfsQgroup::Impl::unknown_id = make_pair(0, 0);


    BtrfsQgroup::Impl::Impl(const id_t& id)
	: Device::Impl(), id(id)
    {
    }


    BtrfsQgroup::Impl::Impl(const xmlNode* node)
	: Device::Impl(node)
    {
	string tmp;

	if (getChildValue(node, "id", tmp))
	    id = parse_id(tmp);

	getChildValue(node, "referenced", referenced);
	getChildValue(node, "exclusive", exclusive);

	if (getChildValue(node, "referenced-limit", tmp))
	    tmp >> referenced_limit;
	if (getChildValue(node, "exclusive-limit", tmp))
	    tmp >> exclusive_limit;
    }


    bool
    BtrfsQgroup::Impl::is_in_view(View view) const
    {
	switch (view)
	{
	    case View::ALL:
		return true;

	    case View::CLASSIC:
		return false;

	    case View::REMOVE:
		return true;
	}

	ST_THROW(LogicException("invalid value for view"));
    }


    void
    BtrfsQgroup::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "id", format_id(id), id != unknown_id);

	setChildValueIf(node, "referenced", referenced, referenced > 0);
	setChildValueIf(node, "exclusive", exclusive, exclusive > 0);

	setChildValue(node, "referenced-limit", referenced_limit);
	setChildValue(node, "exclusive-limit", exclusive_limit);
    }


    string
    BtrfsQgroup::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Btrfs Qgroup").translated;
    }


    string
    BtrfsQgroup::Impl::get_displayname() const
    {
	return format_id(id);
    }


    void
    BtrfsQgroup::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);
    }


    bool
    BtrfsQgroup::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return id == rhs.id && referenced == rhs.referenced && exclusive == rhs.exclusive &&
	    referenced_limit == rhs.referenced_limit && exclusive_limit == rhs.exclusive_limit;
    }


    void
    BtrfsQgroup::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "id", id, rhs.id);

	storage::log_diff(log, "referenced", referenced, rhs.referenced);
	storage::log_diff(log, "exclusive", exclusive, rhs.exclusive);

	storage::log_diff(log, "referenced-limit", referenced_limit, rhs.referenced_limit);
	storage::log_diff(log, "exclusive-limit", exclusive_limit, rhs.exclusive_limit);
    }


    void
    BtrfsQgroup::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);

	out << " id:" << id;

	if (referenced > 0)
	    out << " referenced:" << referenced;
	if (exclusive > 0)
	    out << " exclusive:" << exclusive;

	if (referenced_limit != std::nullopt)
	    out << " referenced-limit:" << referenced_limit.value();
	if (exclusive_limit != std::nullopt)
	    out << " exclusive-limit:" << exclusive_limit.value();
    }


    Btrfs*
    BtrfsQgroup::Impl::get_btrfs()
    {
	vector<Btrfs*> tmp = get_parents_of_type<Btrfs>(View::ALL);
	if (tmp.size() != 1)
	    ST_THROW(Exception("Btrfs parent of BtrfsQgroup broken"));

	return tmp.front();
    }


    const Btrfs*
    BtrfsQgroup::Impl::get_btrfs() const
    {
	vector<const Btrfs*> tmp = get_parents_of_type<const Btrfs>(View::ALL);
	if (tmp.size() != 1)
	    ST_THROW(Exception("Btrfs parent of BtrfsQgroup broken"));

	return tmp.front();
    }


    bool
    BtrfsQgroup::Impl::has_btrfs_subvolume() const
    {
	vector<const BtrfsSubvolume*> tmp = get_parents_of_type<const BtrfsSubvolume>(View::ALL);

	return tmp.size() > 0;
    }


    const BtrfsSubvolume*
    BtrfsQgroup::Impl::get_btrfs_subvolume() const
    {
	vector<const BtrfsSubvolume*> tmp = get_parents_of_type<const BtrfsSubvolume>(View::ALL);

	if (tmp.size() != 1)
	    ST_THROW(Exception("BtrfsSubvolume parent of BtrfsQgroup broken"));

	return tmp.front();
    }


    bool
    BtrfsQgroup::Impl::is_assigned(const BtrfsQgroup* btrfs_qgroup) const
    {
	return get_devicegraph()->holder_exists(btrfs_qgroup->get_sid(), get_sid());
    }


    void
    BtrfsQgroup::Impl::assign(BtrfsQgroup* btrfs_qgroup)
    {
	ST_CHECK_PTR(btrfs_qgroup);

	if (id.first <= btrfs_qgroup->get_id().first)
	    ST_THROW(Exception("level mismatch"));

	BtrfsQgroupRelation::create(get_devicegraph(), btrfs_qgroup, get_non_impl());
    }


    void
    BtrfsQgroup::Impl::unassign(BtrfsQgroup* btrfs_qgroup)
    {
	Holder* holder = get_devicegraph()->find_holder(btrfs_qgroup->get_sid(), get_sid());

	get_devicegraph()->remove_holder(holder);
    }


    vector<BtrfsQgroup*>
    BtrfsQgroup::Impl::get_assigned_btrfs_qgroups()
    {
	return get_children_of_type<BtrfsQgroup>();
    }


    vector<const BtrfsQgroup*>
    BtrfsQgroup::Impl::get_assigned_btrfs_qgroups() const
    {
	return get_children_of_type<const BtrfsQgroup>();
    }


    void
    BtrfsQgroup::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	bool sync_only = false;

	if (id.first == 0)
	{
	    // There are two conditions in which a level 0 qgroup is generated
	    // automatically by btrfs:
	    // 1.) the corresponding subvolume was created
	    // 2.) the corresponding subvolume exists and quota was switched on

	    if (has_btrfs_subvolume())
	    {
		const BtrfsSubvolume* btrfs_subvolume = get_btrfs_subvolume();
		if (!actiongraph.exists_in(btrfs_subvolume, LHS))
		{
		    sync_only = true;
		}
		else
		{
		    const Btrfs* btrfs = get_btrfs();
		    if (!actiongraph.exists_in(btrfs, LHS))
		    {
			sync_only = true;
		    }
		    else if (!to_btrfs(actiongraph.find_device(btrfs->get_sid(), LHS))->has_quota())
		    {
			sync_only = true;
		    }
		}
	    }
	}

	actions.push_back(make_shared<Action::Create>(get_sid(), sync_only));

	if (referenced_limit != std::nullopt || exclusive_limit != std::nullopt)
	    actions.push_back(make_shared<Action::SetLimits>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsQgroup::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	Device::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	vector<shared_ptr<Action::Base>> actions;

	if (lhs.referenced_limit != referenced_limit || lhs.exclusive_limit != exclusive_limit)
	    actions.push_back(make_shared<Action::SetLimits>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsQgroup::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	if (contains(actiongraph.btrfs_qgroup_delete_is_nop, get_btrfs()->get_sid()))
	    return;

	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Delete>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsQgroup::Impl::add_dependencies(Actiongraph::Impl& actiongraph) const
    {
	Device::Impl::add_dependencies(actiongraph);

	// Create actions of qgroups must happen after enabling quota. If the btrfs
	// itself is created the standard dependencies take care.

	const Btrfs* btrfs = get_btrfs();
	if (!actiongraph.exists_in(btrfs, LHS))
	    return;

	map<sid_t, Actiongraph::Impl::vertex_descriptor>::const_iterator it =
	    actiongraph.btrfs_enable_quota.find(btrfs->get_sid());
	if (it == actiongraph.btrfs_enable_quota.end())
	    return;

	for (Actiongraph::Impl::vertex_descriptor vertex1 : actiongraph.actions_with_sid(get_sid(), ONLY_FIRST))
	    actiongraph.add_edge(it->second, vertex1);
    }


    ResizeInfo
    BtrfsQgroup::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    Text
    BtrfsQgroup::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			   // and /dev/sdb2 (2.00 GiB))
			   _("Create qgroup %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			   // and /dev/sdb2 (2.00 GiB))
			   _("Creating qgroup %1$s on %2$s"));

	return sformat(text, format_id(id), get_btrfs()->get_impl().get_message_name());
    }


    void
    BtrfsQgroup::Impl::do_create()
    {
	EnsureMounted ensure_mounted(get_btrfs()->get_top_level_btrfs_subvolume(), false);

	SystemCmd::Args cmd_args = { BTRFS_BIN, "qgroup", "create", format_id(id),
	    ensure_mounted.get_any_mount_point() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    BtrfsQgroup::Impl::do_set_limits_text(const CommitData& commit_data, const Action::SetLimits* action) const
    {
	// For level 0 qgroups the id is unknown if the corresponding subvolume is not
	// created on disk yet. So in that case only referencing the subvolume in the
	// message works. Since creation of level 0 qgroup without a corresponding
	// subvolume is unsupported always use that message for level 0 qgroups. The
	// qgroup for the top level subvolume is again special since it does not have a
	// useful path. Same stuff in BtrfsQgroupRelation.

	if (id.first == 0 && id.second != BtrfsSubvolume::Impl::top_level_id && has_btrfs_subvolume())
	{
	    const BtrfsSubvolume* btrfs_subvolume = get_btrfs_subvolume();

	    Text text = tenser(commit_data.tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Set limits for qgroup of subvolume %1$s on %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Setting limits for qgroup of subvolume %1$s on %2$s"));

	    return sformat(text, btrfs_subvolume->get_path(), get_btrfs()->get_impl().get_message_name());
	}
	else
	{
	    Text text = tenser(commit_data.tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Set limits for qgroup %1$s on %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Setting limits for qgroup %1$s on %2$s"));

	    return sformat(text, format_id(id), get_btrfs()->get_impl().get_message_name());
	}
    }


    void
    BtrfsQgroup::Impl::do_set_limits(CommitData& commit_data, const Action::SetLimits* action)
    {
	EnsureMounted ensure_mounted(get_btrfs()->get_top_level_btrfs_subvolume(), false);

	SystemCmd::Args cmd_args1 = { BTRFS_BIN, "qgroup", "limit", referenced_limit != std::nullopt ?
	    to_string(referenced_limit.value()) : "none", format_id(id), ensure_mounted.get_any_mount_point() };

	SystemCmd cmd1(cmd_args1, SystemCmd::DoThrow);

	SystemCmd::Args cmd_args2 = { BTRFS_BIN, "qgroup", "limit", "-e", exclusive_limit != std::nullopt ?
	    to_string(exclusive_limit.value()) : "none", format_id(id), ensure_mounted.get_any_mount_point() };

	SystemCmd cmd2(cmd_args2, SystemCmd::DoThrow);
    }


    Text
    BtrfsQgroup::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			   // and /dev/sdb2 (2.00 GiB))
			   _("Remove qgroup %1$s on %2$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			   // %2$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			   // and /dev/sdb2 (2.00 GiB))
			   _("Removing qgroup %1$s on %2$s"));

	return sformat(text, format_id(id), get_btrfs()->get_impl().get_message_name());
    }


    void
    BtrfsQgroup::Impl::do_delete() const
    {
	EnsureMounted ensure_mounted(get_btrfs()->get_top_level_btrfs_subvolume(), false);

	SystemCmd::Args cmd_args = { BTRFS_BIN, "qgroup", "destroy", format_id(id),
	    ensure_mounted.get_any_mount_point() };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    BtrfsQgroup::id_t
    BtrfsQgroup::Impl::parse_id(const string& str)
    {
	string::size_type pos = str.find('/');
	if (pos == string::npos)
	    ST_THROW(Exception("failed to parse qgroup failed"));

	id_t id = unknown_id;

	std::istringstream a(str.substr(0, pos));
	a >> id.first;
	if (a.fail() || !a.eof())
	    ST_THROW(Exception("failed to parse qgroup failed"));

	std::istringstream b(str.substr(pos + 1));
	b >> id.second;
	if (b.fail() || !b.eof())
	    ST_THROW(Exception("failed to parse qgroup failed"));

	return id;
    }


    string
    BtrfsQgroup::Impl::format_id(const id_t& id)
    {
	return to_string(id.first) + "/" + to_string(id.second);
    }

}

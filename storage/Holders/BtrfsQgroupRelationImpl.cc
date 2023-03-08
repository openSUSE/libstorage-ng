/*
 * Copyright (c) 2020 SUSE LLC
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


#include "storage/Holders/BtrfsQgroupRelationImpl.h"
#include "storage/Filesystems/BtrfsImpl.h"
#include "storage/Filesystems/BtrfsQgroupImpl.h"
#include "storage/Filesystems/BtrfsSubvolumeImpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Actions/CreateImpl.h"
#include "storage/Actions/DeleteImpl.h"


namespace storage
{

    const char* HolderTraits<BtrfsQgroupRelation>::classname = "BtrfsQgroupRelation";


    BtrfsQgroupRelation::Impl::Impl(const xmlNode* node)
	: Holder::Impl(node)
    {
    }


    void
    BtrfsQgroupRelation::Impl::save(xmlNode* node) const
    {
	Holder::Impl::save(node);
    }


    bool
    BtrfsQgroupRelation::Impl::is_in_view(View view) const
    {
	switch (view)
	{
	    case View::ALL:
		return true;

	    case View::CLASSIC:
		return false;

	    case View::REMOVE:
	    {
		// Follow the relation if the btrfs is removed.
		const Device* source = get_source();
		if (is_btrfs(source))
		    return true;

		// Follow the relation if the qgroup was created along with the btrfs
		// subvolume - in which case the id is 0/0. See bsc #1179590.
		const BtrfsQgroup* target = to_btrfs_qgroup(get_target());
		if (target->get_id() == BtrfsQgroup::Impl::unknown_id)
		    return true;

		return false;
	    }
	}

	ST_THROW(LogicException("invalid value for view"));
    }


    const Btrfs*
    BtrfsQgroupRelation::Impl::get_btrfs() const
    {
	return to_btrfs_qgroup(get_target())->get_btrfs();
    }


    void
    BtrfsQgroupRelation::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	// Only btrfs qgroup relations between btrfs qgroups must be created in the
	// system. Relations from a btrfs or from a subvolume do not exists in the system.

	vector<shared_ptr<Action::Base>> actions;

	if (is_btrfs_qgroup(get_source()) && is_btrfs_qgroup(get_target()))
	    actions.push_back(make_shared<Action::Create>(make_pair(get_source_sid(), get_target_sid())));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsQgroupRelation::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	if (contains(actiongraph.btrfs_qgroup_delete_is_nop, get_btrfs()->get_sid()))
	    return;

	// See above.

	vector<shared_ptr<Action::Base>> actions;

	if (is_btrfs_qgroup(get_source()) && is_btrfs_qgroup(get_target()))
	    actions.push_back(make_shared<Action::Delete>(make_pair(get_source_sid(), get_target_sid())));

	actiongraph.add_chain(actions);
    }


    void
    BtrfsQgroupRelation::Impl::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex1,
						Actiongraph::Impl& actiongraph) const
    {
	// qgroup relations must be created after the qgroups have been created. Inverse
	// order for delete. This is likely so generic that it could be move to Holder.

	const Action::Base* action1 = actiongraph[vertex1];

	if (is_create(action1))
	{
	    for (Actiongraph::Impl::vertex_descriptor vertex2 : actiongraph.actions_with_sid(get_source_sid(), ONLY_LAST))
		actiongraph.add_edge(vertex2, vertex1);

	    for (Actiongraph::Impl::vertex_descriptor vertex2 : actiongraph.actions_with_sid(get_target_sid(), ONLY_LAST))
		actiongraph.add_edge(vertex2, vertex1);
	}

	if (is_delete(action1))
	{
	    for (Actiongraph::Impl::vertex_descriptor vertex2 : actiongraph.actions_with_sid(get_source_sid(), ONLY_FIRST))
		actiongraph.add_edge(vertex1, vertex2);

	    for (Actiongraph::Impl::vertex_descriptor vertex2 : actiongraph.actions_with_sid(get_target_sid(), ONLY_FIRST))
		actiongraph.add_edge(vertex1, vertex2);
	}
    }


    bool
    BtrfsQgroupRelation::Impl::equal(const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	return Holder::Impl::equal(rhs);
    }


    void
    BtrfsQgroupRelation::Impl::log_diff(std::ostream& log, const Holder::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Holder::Impl::log_diff(log, rhs);
    }


    void
    BtrfsQgroupRelation::Impl::print(std::ostream& out) const
    {
	Holder::Impl::print(out);
    }


    Text
    BtrfsQgroupRelation::Impl::do_create_text(Tense tense) const
    {
	const BtrfsQgroup* qgroup1 = to_btrfs_qgroup(get_source());
	const BtrfsQgroup* qgroup2 = to_btrfs_qgroup(get_target());
	const Btrfs* btrfs = qgroup1->get_btrfs();

	if (qgroup1->get_id().first == 0 && qgroup1->get_id().second != BtrfsSubvolume::Impl::top_level_id &&
	    qgroup1->get_impl().has_btrfs_subvolume())
	{
	    const BtrfsSubvolume* btrfs_subvolume = qgroup1->get_impl().get_btrfs_subvolume();

	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Assign qgroup of subvolume %1$s to qgroup %2$s on %3$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Assigning qgroup of subvolume %1$s to qgroup %2$s on %3$s"));

	    return sformat(text, btrfs_subvolume->get_path(), BtrfsQgroup::Impl::format_id(qgroup2->get_id()),
			   btrfs->get_impl().get_message_name());
	}
	else
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 2/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Assign qgroup %1$s to qgroup %2$s on %3$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 2/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Assigning qgroup %1$s to qgroup %2$s on %3$s"));

	    return sformat(text, BtrfsQgroup::Impl::format_id(qgroup1->get_id()),
			   BtrfsQgroup::Impl::format_id(qgroup2->get_id()), btrfs->get_impl().get_message_name());
	}
    }


    void
    BtrfsQgroupRelation::Impl::do_create()
    {
	const BtrfsQgroup* qgroup1 = to_btrfs_qgroup(get_source());
	const BtrfsQgroup* qgroup2 = to_btrfs_qgroup(get_target());
	const Btrfs* btrfs = qgroup1->get_btrfs();

	EnsureMounted ensure_mounted(btrfs->get_top_level_btrfs_subvolume(), false);

	string cmd_line = BTRFS_BIN " qgroup assign " + BtrfsQgroup::Impl::format_id(qgroup1->get_id()) + " " +
	    BtrfsQgroup::Impl::format_id(qgroup2->get_id()) + " " + quote(ensure_mounted.get_any_mount_point());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    BtrfsQgroupRelation::Impl::do_delete_text(Tense tense) const
    {
	const BtrfsQgroup* qgroup1 = to_btrfs_qgroup(get_source());
	const BtrfsQgroup* qgroup2 = to_btrfs_qgroup(get_target());
	const Btrfs* btrfs = qgroup1->get_btrfs();

	if (qgroup1->get_id().first == 0 && qgroup1->get_id().second != BtrfsSubvolume::Impl::top_level_id &&
	    qgroup1->get_impl().has_btrfs_subvolume())
	{
	    const BtrfsSubvolume* btrfs_subvolume = qgroup1->get_impl().get_btrfs_subvolume();

	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Unassign qgroup of subvolume %1$s from qgroup %2$s on %3$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by subvolume path (e.g. var/log),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Unassigning qgroup of subvolume %1$s from qgroup %2$s on %3$s"));

	    return sformat(text, btrfs_subvolume->get_path(), BtrfsQgroup::Impl::format_id(qgroup2->get_id()),
			   btrfs->get_impl().get_message_name());
	}
	else
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 2/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Unassign qgroup %1$s from qgroup %2$s on %3$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by the btrfs qgroup id (e.g. 1/0),
			       // %2$s is replaced by the btrfs qgroup id (e.g. 2/0),
			       // %3$s is replaced by one or more devices (e.g /dev/sda1 (2.00 GiB)
			       // and /dev/sdb2 (2.00 GiB))
			       _("Unassigning qgroup %1$s from qgroup %2$s on %3$s"));

	    return sformat(text, BtrfsQgroup::Impl::format_id(qgroup1->get_id()),
			   BtrfsQgroup::Impl::format_id(qgroup2->get_id()), btrfs->get_impl().get_message_name());
	}
    }


    void
    BtrfsQgroupRelation::Impl::do_delete() const
    {
	const BtrfsQgroup* qgroup1 = to_btrfs_qgroup(get_source());
	const BtrfsQgroup* qgroup2 = to_btrfs_qgroup(get_target());
	const Btrfs* btrfs = qgroup1->get_btrfs();

	EnsureMounted ensure_mounted(btrfs->get_top_level_btrfs_subvolume(), false);

	string cmd_line = BTRFS_BIN " qgroup remove " + BtrfsQgroup::Impl::format_id(qgroup1->get_id()) + " " +
	    BtrfsQgroup::Impl::format_id(qgroup2->get_id()) + " " + quote(ensure_mounted.get_any_mount_point());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}

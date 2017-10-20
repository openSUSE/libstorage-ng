/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2017] SUSE LLC
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


#include "storage/Utils/HumanString.h"
#include "storage/Devices/ImplicitPtImpl.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/AlignmentImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<ImplicitPt>::classname = "ImplicitPt";


    ImplicitPt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node)
    {
    }


    void
    ImplicitPt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);
    }


    void
    ImplicitPt::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	PartitionTable::Impl::check(check_callbacks);
    }


    Region
    ImplicitPt::Impl::get_usable_region() const
    {
	return get_partitionable()->get_region();
    }


    bool
    ImplicitPt::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return true;
    }


    void
    ImplicitPt::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);
    }


    void
    ImplicitPt::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);
    }


    bool
    ImplicitPt::Impl::is_partition_type_supported(PartitionType type) const
    {
	return type == PartitionType::PRIMARY;
    }


    bool
    ImplicitPt::Impl::is_partition_id_supported(unsigned int id) const
    {
	static const vector<unsigned int> supported_ids = {
	    ID_UNKNOWN, ID_LINUX
	};

	return contains(supported_ids, id);
    }


    unsigned int
    ImplicitPt::Impl::max_primary() const
    {
	return min(1U, get_partitionable()->get_range());
    }


    void
    ImplicitPt::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid(), false, true));

	actiongraph.add_chain(actions);
    }


    void
    ImplicitPt::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(get_sid(), false, true));

	actiongraph.add_chain(actions);
    }


    Text
    ImplicitPt::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/dasda)
			   _("Create implicit partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/dasda)
			   _("Creating implicit partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }


    Text
    ImplicitPt::Impl::do_delete_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/dasda)
			   _("Delete implicit partition table on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/dasda)
			   _("Deleting implicit partition table on %1$s"));

	return sformat(text, partitionable->get_displayname().c_str());
    }

}

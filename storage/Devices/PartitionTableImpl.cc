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


#include <boost/algorithm/string.hpp>

#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/MsdosImpl.h"
#include "storage/Holders/Subdevice.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Utils/AlignmentImpl.h"
#include "storage/Utils/Algorithm.h"
#include "storage/Prober.h"
#include "storage/Utils/StorageDefines.h"


namespace storage
{

    using namespace std;


    const vector<string> EnumTraits<PtType>::names({
	"unknown", "loop", "MS-DOS", "GPT", "DASD", "Mac", "implicit"
    });


    const char* DeviceTraits<PartitionTable>::classname = "PartitionTable";


    PartitionTable::Impl::Impl(const xmlNode* node)
	: Device::Impl(node), read_only(false)
    {
	getChildValue(node, "read-only", read_only);
    }


    void
    PartitionTable::Impl::probe_pass_1c(Prober& prober)
    {
	Device::Impl::probe_pass_1c(prober);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = prober.get_system_info().getParted(partitionable->get_name());

	for (const Parted::Entry& entry : parted.get_entries())
	{
	    string name = partitionable->get_impl().partition_name(entry.number);

	    Partition* p = create_partition(name, entry.region, entry.type);
	    p->get_impl().probe_pass_1a(prober);
	}
    }


    void
    PartitionTable::Impl::save(xmlNode* node) const
    {
	Device::Impl::save(node);

	setChildValueIf(node, "read-only", read_only, read_only);
    }


    void
    PartitionTable::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Device::Impl::check(check_callbacks);

	const Device* parent = get_single_parent_of_type<const Device>();

	if (!is_partitionable(parent))
	    ST_THROW(Exception("parent of partition table is not a partitionable"));
    }


    Partition*
    PartitionTable::Impl::create_partition(const string& name, const Region& region, PartitionType type)
    {
	const Region& partitionable_region = get_partitionable()->get_region();
	if (region.get_block_size() != partitionable_region.get_block_size())
	    ST_THROW(DifferentBlockSizes(region.get_block_size(), partitionable_region.get_block_size()));

	const Device* parent = type == PartitionType::LOGICAL ? get_extended() : get_non_impl();

	Partition* partition = Partition::create(get_devicegraph(), name, region, type);
	Subdevice::create(get_devicegraph(), parent, partition);

	partition->get_impl().update_sysfs_name_and_path();
	partition->get_impl().update_udev_paths_and_ids();

	if (boost::starts_with(name, DEVMAPPERDIR "/"))
	    partition->set_dm_table_name(name.substr(strlen(DEVMAPPERDIR "/")));

	return partition;
    }


    Partition*
    PartitionTable::Impl::get_partition(const string& name)
    {
	for (Partition* partition : get_partitions())
	{
	    if (partition->get_name() == name)
		return partition;
	}

	ST_THROW(Exception("partition not found"));
    }


    void
    PartitionTable::Impl::delete_partition(Partition* partition)
    {
	PartitionType old_type = partition->get_type();
	unsigned int old_number = partition->get_number();

	partition->remove_descendants();

	get_devicegraph()->remove_device(partition);

	if ((get_type() == PtType::GPT || get_type() == PtType::MSDOS) &&
	    (old_type == PartitionType::PRIMARY || old_type == PartitionType::EXTENDED))
	{
	    // After deleting a primary or extended partition on MS-DOS or GPT
	    // the numbers of primary and extended partitions not on disk with
	    // higher numbers are shifted. For DASD there is special handling.

	    vector<Partition*> partitions = get_partitions();
	    sort(partitions.begin(), partitions.end(), Partition::compare_by_number);
	    for (Partition* tmp : partitions)
	    {
		PartitionType type = tmp->get_type();
		unsigned int number = tmp->get_number();

		if ((type == PartitionType::PRIMARY || type == PartitionType::EXTENDED) &&
		    number > old_number && !tmp->exists_in_probed())
		{
		    tmp->get_impl().set_number(old_number);
		    old_number = number;
		}
	    }
	}
    }


    void
    PartitionTable::Impl::delete_partition(const string& name)
    {
	delete_partition(get_partition(name));
    }


    Partitionable*
    PartitionTable::Impl::get_partitionable()
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[vertex]);
    }


    const Partitionable*
    PartitionTable::Impl::get_partitionable() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[vertex]);
    }


    vector<Partition*>
    PartitionTable::Impl::get_partitions()
    {
	Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<Partition>(devicegraph.children(vertex));
    }


    vector<const Partition*>
    PartitionTable::Impl::get_partitions() const
    {
	const Devicegraph::Impl& devicegraph = get_devicegraph()->get_impl();
	Devicegraph::Impl::vertex_descriptor vertex = get_vertex();

	return devicegraph.filter_devices_of_type<Partition>(devicegraph.children(vertex));
    }


    bool
    PartitionTable::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Device::Impl::equal(rhs))
	    return false;

	return read_only == rhs.read_only;
    }


    void
    PartitionTable::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Device::Impl::log_diff(log, rhs);

	storage::log_diff(log, "read-only", read_only, rhs.read_only);
    }


    void
    PartitionTable::Impl::print(std::ostream& out) const
    {
	Device::Impl::print(out);
    }


    unsigned int
    PartitionTable::Impl::num_primary() const
    {
	vector<const Partition*> partitions = get_partitions();
	return count_if(partitions.begin(), partitions.end(), [](const Partition* partition) {
	    return partition->get_type() == PartitionType::PRIMARY;
	});
    }


    const Partition*
    PartitionTable::Impl::get_extended() const
    {
	ST_THROW(Exception("has no extended partition"));
    }


    Alignment
    PartitionTable::Impl::get_alignment(AlignType align_type) const
    {
	return Alignment(get_partitionable()->get_topology(), align_type);
    }


    vector<PartitionSlot>
    PartitionTable::Impl::get_unused_partition_slots(AlignPolicy align_policy,
						     AlignType align_type) const
    {
	const Partitionable* partitionable = get_partitionable();
	const Alignment alignment = get_alignment(align_type);

	bool is_primary_possible = num_primary() + (has_extended() ? 1 : 0) < max_primary();
	bool is_extended_possible = is_primary_possible && extended_possible() && !has_extended();
	bool is_logical_possible = has_extended() && num_logical() < (max_logical() - max_primary());

	vector<PartitionSlot> slots;

	vector<const Partition*> partitions = get_partitions();
	sort(partitions.begin(), partitions.end(), Partition::compare_by_number);

	if (true)
	{
	    PartitionSlot slot;

	    if (get_type() != PtType::DASD)
	    {
		slot.number = first_missing_number(partitions, 1);

		slot.name = partitionable->get_impl().partition_name(slot.number);
	    }

	    slot.primary_slot = true;
	    slot.primary_possible = is_primary_possible;
	    slot.extended_slot = extended_possible();
	    slot.extended_possible = is_extended_possible;
	    slot.logical_slot = false;
	    slot.logical_possible = false;

	    vector<Region> used_regions;
	    for (const Partition* partition : partitions)
	    {
		if (partition->get_type() != PartitionType::LOGICAL)
		    used_regions.push_back(partition->get_region());
	    }

	    Region usable_region = get_usable_region();
	    for (const Region& unused_region : usable_region.unused_regions(used_regions))
	    {
		slot.region = unused_region;

		if (alignment.get_impl().align_region_in_place(slot.region, align_policy))
		{
		    // For DASDs the slot number is the number of partitions/used regions
		    // before the slot.

		    if (get_type() == PtType::DASD)
		    {
			slot.number = count_if(used_regions.begin(), used_regions.end(),
					       [&slot](const Region& used_region) {
						   return used_region < slot.region;
					       }) + 1;

			slot.name = partitionable->get_impl().partition_name(slot.number);
		    }

		    slots.push_back(slot);
		}
	    }
	}

	if (has_extended())
	{
	    PartitionSlot slot;

	    slot.number = max_primary() + num_logical() + 1;
	    slot.name = partitionable->get_impl().partition_name(slot.number);

	    slot.primary_slot = false;
	    slot.primary_possible = false;
	    slot.extended_slot = false;
	    slot.extended_possible = false;
	    slot.logical_slot = true;
	    slot.logical_possible = is_logical_possible;

	    vector<Region> used_regions;
	    for (const Partition* partition : partitions)
	    {
		if (partition->get_type() == PartitionType::LOGICAL)
		    used_regions.push_back(partition->get_region());
	    }

	    const Region& extended_region = get_extended()->get_region();
	    for (const Region& unused_region : extended_region.unused_regions(used_regions))
	    {
		// Keep space for EBRs.

		if (unused_region.get_length() <= Msdos::Impl::num_ebrs)
		    continue;

		Region adjusted_region = unused_region;
		adjusted_region.adjust_start(+Msdos::Impl::num_ebrs);
		adjusted_region.adjust_length(-Msdos::Impl::num_ebrs);

		slot.region = adjusted_region;
		if (alignment.get_impl().align_region_in_place(slot.region, align_policy))
		{
		    slots.push_back(slot);
		}
	    }
	}

	y2deb("slots:" << slots);

	return slots;
    }


    Region
    PartitionTable::Impl::align(const Region& region, AlignPolicy align_policy,
				AlignType align_type) const
    {
	return get_alignment(align_type).align(region, align_policy);
    }


    /**
     * Returns input sorted by the default comparison of the return value of
     * the key function fnc of each value in input.
     */
    template<typename ValueType, typename KeyType>
    vector<ValueType>
    sort_by_key(const vector<ValueType>& input, std::function<KeyType(ValueType)> fnc)
    {
	typedef typename vector<ValueType>::size_type size_type;

	map<KeyType, size_type> tmp;

	for (size_type i = 0; i < input.size(); ++i)
	    tmp[fnc(input[i])] = i;

	vector<ValueType> output;

	for (const auto& t : tmp)
	    output.push_back(input[t.second]);

	return output;
    }


    void
    PartitionTable::Impl::run_dependency_manager(Actiongraph::Impl& actiongraph)
    {
	// To speed up things this fuctions finds the actions for all
	// partition tables. Going through the actions for every partition
	// table individually is slow.

	struct AllActions
	{
	    vector<Actiongraph::Impl::vertex_descriptor> shrink_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> delete_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> rename_in_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> create_actions;
	    vector<Actiongraph::Impl::vertex_descriptor> grow_actions;
	};

	// Build map with all actions for all partition tables.

	map<sid_t, AllActions> all_actions_per_partition_table;

	for (Actiongraph::Impl::vertex_descriptor vertex : actiongraph.vertices())
	{
	    const Action::Base* action = actiongraph[vertex];

	    const Action::Create* create_action = dynamic_cast<const Action::Create*>(action);
	    if (create_action)
	    {
		const Device* device = create_action->get_device(actiongraph);
		if (is_partition(device))
		{
		    const Partition* partition = to_partition(device);
		    sid_t sid = partition->get_partition_table()->get_sid();

		    all_actions_per_partition_table[sid].create_actions.push_back(vertex);
		}
	    }

	    const Action::Delete* delete_action = dynamic_cast<const Action::Delete*>(action);
            if (delete_action)
            {
                const Device* device = delete_action->get_device(actiongraph);
                if (is_partition(device))
                {
                    const Partition* partition = to_partition(device);
                    sid_t sid = partition->get_partition_table()->get_sid();

                    all_actions_per_partition_table[sid].delete_actions.push_back(vertex);
                }
            }

	    const Action::Resize* resize_action = dynamic_cast<const Action::Resize*>(action);
	    if (resize_action)
	    {
		const Device* device = resize_action->get_device(actiongraph, RHS);
		if (is_partition(device))
		{
		    const Partition* partition = to_partition(device);
		    sid_t sid = partition->get_partition_table()->get_sid();

		    if (resize_action->resize_mode == ResizeMode::GROW)
			all_actions_per_partition_table[sid].grow_actions.push_back(vertex);
		    else
			all_actions_per_partition_table[sid].shrink_actions.push_back(vertex);
		}
	    }

	    const Action::RenameIn* rename_in_action = dynamic_cast<const Action::RenameIn*>(action);
	    if (rename_in_action)
	    {
		const Partition* partition = to_partition(rename_in_action->get_renamed_blk_device(actiongraph, RHS));
		sid_t sid = partition->get_partition_table()->get_sid();

		all_actions_per_partition_table[sid].rename_in_actions.push_back(vertex);
	    }
	}

	// Some functions used for sorting actions by partition number.

        const Devicegraph* devicegraph_rhs = actiongraph.get_devicegraph(RHS);
        const Devicegraph* devicegraph_lhs = actiongraph.get_devicegraph(LHS);

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc1 =
	    [&actiongraph, &devicegraph_lhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Partition* partition = to_partition(devicegraph_lhs->find_device(action->sid));
	    return partition->get_number();
	};

	std::function<int(Actiongraph::Impl::vertex_descriptor)> key_fnc2 =
	    [&actiongraph, &devicegraph_lhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::RenameIn* action = dynamic_cast<const Action::RenameIn*>(actiongraph[vertex]);
	    const Partition* partition_lhs = to_partition(action->get_renamed_blk_device(actiongraph, LHS));
	    const Partition* partition_rhs = to_partition(action->get_renamed_blk_device(actiongraph, RHS));
	    unsigned int number_lhs = partition_lhs->get_number();
	    unsigned int number_rhs = partition_rhs->get_number();
	    return number_lhs > number_rhs ? number_rhs : - number_lhs;
	};

	std::function<unsigned int(Actiongraph::Impl::vertex_descriptor)> key_fnc3 =
	    [&actiongraph, &devicegraph_rhs](Actiongraph::Impl::vertex_descriptor vertex) {
	    const Action::Base* action = actiongraph[vertex];
	    const Partition* partition = to_partition(devicegraph_rhs->find_device(action->sid));
	    return partition->get_number();
	};

	// Iterate over all partition tables with actions and build chain of
	// dependencies.

	for (auto& tmp : all_actions_per_partition_table)
	{
	    AllActions& all_actions = tmp.second;

	    vector<Actiongraph::Impl::vertex_descriptor> actions;

	    all_actions.shrink_actions = sort_by_key(all_actions.shrink_actions, key_fnc1);
	    actions.insert(actions.end(), all_actions.shrink_actions.begin(),
			   all_actions.shrink_actions.end());

	    all_actions.delete_actions = sort_by_key(all_actions.delete_actions, key_fnc1);
	    actions.insert(actions.end(), all_actions.delete_actions.rbegin(),
			   all_actions.delete_actions.rend());

	    all_actions.rename_in_actions = sort_by_key(all_actions.rename_in_actions, key_fnc2);
	    actions.insert(actions.end(), all_actions.rename_in_actions.begin(),
			   all_actions.rename_in_actions.end());

	    all_actions.create_actions = sort_by_key(all_actions.create_actions, key_fnc3);
	    actions.insert(actions.end(), all_actions.create_actions.begin(),
			   all_actions.create_actions.end());

	    all_actions.grow_actions = sort_by_key(all_actions.grow_actions, key_fnc3);
	    actions.insert(actions.end(), all_actions.grow_actions.begin(),
			   all_actions.grow_actions.end());

	    actiongraph.add_chain(actions);

	    if (devicegraph_rhs->device_exists(tmp.first))
	    {
		const PartitionTable* partition_table = to_partition_table(devicegraph_rhs->find_device(tmp.first));
		actiongraph.last_action_on_partition_table[partition_table->get_sid()] = actions.back();
	    }
	}
    }


    void
    PartitionTable::Impl::do_delete() const
    {
	const Partitionable* partitionable = get_partitionable();

	// TODO does not work for DASD partition table, see
	// https://bugzilla.suse.com/show_bug.cgi?id=896485

	partitionable->get_impl().wipe_device();
    }

}

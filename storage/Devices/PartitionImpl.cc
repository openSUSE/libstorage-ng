/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2021] SUSE LLC
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

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/Gpt.h"
#include "storage/Devices/MsdosImpl.h"
#include "storage/Devices/DasdPt.h"
#include "storage/Devices/ImplicitPt.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Filesystems/MountPoint.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Storage.h"
#include "storage/FreeInfo.h"
#include "storage/Utils/XmlFile.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partition>::classname = "Partition";


    // strings must match what parted understands
    const vector<string> EnumTraits<PartitionType>::names({
	"primary", "extended", "logical"
    });


    Partition::Impl::Impl(const string& name, const Region& region, PartitionType type)
	: BlkDevice::Impl(name, region), type(type), id(default_id_for_type(type))
    {
    }


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
	string tmp;

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, PartitionType::PRIMARY);
	getChildValue(node, "id", id);
	getChildValue(node, "boot", boot);
	getChildValue(node, "legacy-boot", legacy_boot);

	getChildValue(node, "label", label);
	getChildValue(node, "uuid", uuid);
    }


    string
    Partition::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Partition").translated;
    }


    string
    Partition::Impl::get_name_sort_key() const
    {
	const Partitionable* partitionable = get_partitionable();

	string sort_key = partitionable->get_impl().get_name_sort_key();
	if (isdigit(sort_key.back()))
	    sort_key += 'p';
	sort_key += pad_front(to_string(get_number()), 3, '0');

	return sort_key;
    }


    void
    Partition::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	SystemInfo::Impl& system_info = prober.get_system_info();

	const PartitionTable* partition_table = get_partition_table();
	const Partitionable* partitionable = partition_table->get_partitionable();

	const Parted& parted = system_info.getParted(partitionable->get_name());
	Parted::Entry entry;
	if (!parted.get_entry(get_number(), entry))
	    ST_THROW(Exception("partition not found in parted output"));

	id = entry.id;
	boot = entry.boot;
	legacy_boot = entry.legacy_boot;
	label = entry.name;

	if (!is_active())
	    return;

	const CmdUdevadmInfo& cmd_udevadm_info = system_info.getCmdUdevadmInfo(get_name());

	// Use partition UUID only on GPT. On MS-DOS it is also
	// available but includes the partition number, so updating it
	// would be required (even in fstab) when a logical partition
	// gets renumbered. For UUIDs a rather odd behaviour.

	if (!cmd_udevadm_info.get_by_part_uuid_links().empty() && is_gpt(partition_table))
	    uuid = cmd_udevadm_info.get_by_part_uuid_links().front();

	probe_topology(prober);
    }


    void
    Partition::Impl::probe_topology(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();

	const Partitionable* partitionable = get_partitionable();

	const File& alignment_offset_file = get_sysfs_file(system_info, "alignment_offset");
	const File& optimal_io_size_file = partitionable->get_impl().get_sysfs_file(system_info,
										    "queue/optimal_io_size");

	unsigned long long alignment_offset = alignment_offset_file.get<int>();
	unsigned long long optimal_io_size = optimal_io_size_file.get<int>();

	set_topology(Topology(alignment_offset, optimal_io_size));
    }


    void
    Partition::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "type", toString(type));
	setChildValueIf(node, "id", sformat("0x%02x", id), id != 0);
	setChildValueIf(node, "boot", boot, boot);
	setChildValueIf(node, "legacy-boot", legacy_boot, legacy_boot);

	setChildValueIf(node, "label", label, !label.empty());
	setChildValueIf(node, "uuid", uuid, !uuid.empty());
    }


    bool
    Partition::Impl::is_usable_as_blk_device() const
    {
	if (!BlkDevice::Impl::is_usable_as_blk_device())
	    return false;

	return type == PartitionType::PRIMARY || type == PartitionType::LOGICAL;
    }


    vector<MountByType>
    Partition::Impl::possible_mount_bys() const
    {
	return get_partitionable()->get_impl().possible_mount_bys();
    }


    void
    Partition::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	BlkDevice::Impl::check(check_callbacks);

	const Device* parent = get_single_parent_of_type<const Device>();

	switch (type)
	{
	    case PartitionType::PRIMARY:
		if (!is_partition_table(parent))
		    ST_THROW(Exception("parent of primary partition is not partition table"));
		break;

	    case PartitionType::EXTENDED:
		if (!is_partition_table(parent))
		    ST_THROW(Exception("parent of extended partition is not partition table"));
		break;

	    case PartitionType::LOGICAL:
		if (!is_partition(parent) || to_partition(parent)->get_type() != PartitionType::EXTENDED)
		    ST_THROW(Exception("parent of logical partition is not extended partition"));
		break;
	}
    }


    unsigned int
    Partition::Impl::get_number() const
    {
	return device_to_name_and_number(get_name()).second;
    }


    void
    Partition::Impl::set_number(unsigned int number)
    {
	const Partitionable* partitionable = get_partitionable();

	set_name(partitionable->partition_name(number));

	update_sysfs_name_and_path();
	update_udev_paths_and_ids();
    }


    void
    Partition::Impl::set_region(const Region& region)
    {
	const Region& partitionable_region = get_partitionable()->get_region();
	if (region.get_block_size() != partitionable_region.get_block_size())
	    ST_THROW(DifferentBlockSizes(region.get_block_size(), partitionable_region.get_block_size()));

	BlkDevice::Impl::set_region(region);

	calculate_topology();
    }


    void
    Partition::Impl::calculate_topology()
    {
	// TODO The alignment_offset should also be calculated. But
	// since all new partition are normally aligned anyway this is
	// not so urgent. Would require to probe and save the
	// physical_block_size.

	const Partitionable* partitionable = get_partitionable();

	unsigned long long alignment_offset = 0;
	unsigned long optimal_io_size = partitionable->get_topology().get_optimal_io_size();

	set_topology(Topology(alignment_offset, optimal_io_size));
    }


    PartitionTable*
    Partition::Impl::get_partition_table()
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	if (type == PartitionType::LOGICAL)
	    vertex = get_devicegraph()->get_impl().parent(vertex);

	return to_partition_table(get_devicegraph()->get_impl()[vertex]);
    }


    const PartitionTable*
    Partition::Impl::get_partition_table() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	if (type == PartitionType::LOGICAL)
	    vertex = get_devicegraph()->get_impl().parent(vertex);

	return to_partition_table(get_devicegraph()->get_impl()[vertex]);
    }


    Partitionable*
    Partition::Impl::get_partitionable()
    {
	PartitionTable* partition_table = get_partition_table();

	return partition_table->get_partitionable();
    }


    const Partitionable*
    Partition::Impl::get_partitionable() const
    {
	const PartitionTable* partition_table = get_partition_table();

	return partition_table->get_partitionable();
    }


    void
    Partition::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	bool nop = is_implicit_pt(get_partition_table());

	actions.push_back(make_shared<Action::Create>(get_sid(), false, nop));

	if (default_id_for_type(type) != id)
	{
	    // For some partition ids it is fine to skip do_set_id() since
	    // do_create() already sets the partition id correctly.

	    static const vector<unsigned int> skip_ids = {
		ID_LINUX, ID_SWAP, ID_DOS32, ID_NTFS, ID_WINDOWS_BASIC_DATA
	    };

	    if (!contains(skip_ids, id))
		actions.push_back(make_shared<Action::SetPartitionId>(get_sid()));
	}

	if (!label.empty())
	    actions.push_back(make_shared<Action::SetPartitionLabel>(get_sid()));

	if (boot)
	    actions.push_back(make_shared<Action::SetBoot>(get_sid()));

	if (legacy_boot)
	    actions.push_back(make_shared<Action::SetLegacyBoot>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Partition::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_type() != lhs.get_type())
	{
	    ST_THROW(Exception("cannot change partition type"));
	}

	if (get_region().get_start() != lhs.get_region().get_start())
	{
	    ST_THROW(Exception("cannot move partition"));
	}

	if (get_id() != lhs.get_id())
	{
	    shared_ptr<Action::Base> action = make_shared<Action::SetPartitionId>(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (label != lhs.label)
	{
	    shared_ptr<Action::Base> action = make_shared<Action::SetPartitionLabel>(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (boot != lhs.boot)
	{
	    shared_ptr<Action::Base> action = make_shared<Action::SetBoot>(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (legacy_boot != lhs.legacy_boot)
	{
	    shared_ptr<Action::Base> action = make_shared<Action::SetLegacyBoot>(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    void
    Partition::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	bool nop = is_implicit_pt(get_partition_table());

	actions.push_back(make_shared<Action::Delete>(get_sid(), false, nop));

	actiongraph.add_chain(actions);
    }


    bool
    Partition::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return type == rhs.type && id == rhs.id && boot == rhs.boot &&
	    legacy_boot == rhs.legacy_boot && label == rhs.label && uuid == rhs.uuid;
    }


    void
    Partition::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "type", type, rhs.type);
	storage::log_diff_hex(log, "id", id, rhs.id);
	storage::log_diff(log, "boot", boot, rhs.boot);
	storage::log_diff(log, "legacy-boot", legacy_boot, rhs.legacy_boot);

	storage::log_diff(log, "label", label, rhs.label);
	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    Partition::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " type:" << toString(type)
	    << " id:" << sformat("0x%02x", id);

	if (boot)
	    out << " boot";

	if (legacy_boot)
	    out << " legacy-boot";

	if (!label.empty())
	    out << " label:" << label;

	if (!uuid.empty())
	    out << " uuid:" << uuid;
    }


    void
    Partition::Impl::process_udev_paths(vector<string>& udev_paths) const
    {
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().process_udev_paths(udev_paths);
    }


    void
    Partition::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().process_udev_ids(udev_ids);
    }


    void
    Partition::Impl::set_type(PartitionType type)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->get_impl().is_partition_type_supported(type))
	    ST_THROW(Exception(sformat("illegal partition type %s on %s", toString(type),
				       toString(partition_table->get_type()))));

	Impl::type = type;
    }


    void
    Partition::Impl::set_id(unsigned int id)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->is_partition_id_supported(id))
	    ST_THROW(Exception(sformat("illegal partition id %d on %s", id,
				       toString(partition_table->get_type()))));

	Impl::id = id;
    }


    void
    Partition::Impl::set_boot(bool boot)
    {
	PartitionTable* partition_table = get_partition_table();

	if (!partition_table->is_partition_boot_flag_supported())
	    ST_THROW(Exception(sformat("set_boot not supported on %s",
				       toString(partition_table->get_type()))));

	if (boot && !Impl::boot)
	{
	    for (Partition* partition : partition_table->get_partitions())
		partition->get_impl().boot = false;
	}

	Impl::boot = boot;
    }


    void
    Partition::Impl::set_legacy_boot(bool legacy_boot)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->is_partition_legacy_boot_flag_supported())
	    ST_THROW(Exception(sformat("set_legacy_boot not supported on %s",
				       toString(partition_table->get_type()))));

	Impl::legacy_boot = legacy_boot;
    }


    void
    Partition::Impl::update_name()
    {
	const Partitionable* partitionable = get_partitionable();

	set_name(partitionable->partition_name(get_number()));
    }


    void
    Partition::Impl::update_sysfs_name_and_path()
    {
	const Partitionable* partitionable = get_partitionable();

	if ((!partitionable->get_sysfs_name().empty() && !partitionable->get_sysfs_path().empty()) &&
	    partitionable->get_dm_table_name().empty())
	{
	    set_sysfs_name(partitionable->get_sysfs_name() + to_string(get_number()));
	    set_sysfs_path(partitionable->get_sysfs_path() + "/" + get_sysfs_name());
	}
	else
	{
	    set_sysfs_name("");
	    set_sysfs_path("");
	}
    }


    void
    Partition::Impl::update_udev_paths_and_ids()
    {
	const Partitionable* partitionable = get_partitionable();

	string addition = "-part" + to_string(get_number());

	vector<string> udev_paths;
	for (const string& udev_path : partitionable->get_udev_paths())
	{
	    udev_paths.push_back(udev_path + addition);
	}
	set_udev_paths(udev_paths);

	vector<string> udev_ids;
	for (const string& udev_id : partitionable->get_udev_ids())
	{
	    // The partition link for udev id links starting with dm-uuid is
	    // special for multipath and dmraid (see bsc #1099394). Handling
	    // dmraid here is optional since the links are not whitelisted.

	    if (boost::starts_with(udev_id, "dm-uuid-mpath") ||
		boost::starts_with(udev_id, "dm-uuid-DMRAID"))
		udev_ids.push_back("dm-uuid" + addition + udev_id.substr(strlen("dm-uuid")));
	    else
		udev_ids.push_back(udev_id + addition);
	}
	set_udev_ids(udev_ids);
    }


    ResizeInfo
    Partition::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	if (is_implicit_pt(get_partition_table()))
	{
	    return ResizeInfo(false, RB_ON_IMPLICIT_PARTITION_TABLE);
	}

	if (type == PartitionType::EXTENDED)
	{
	    // TODO resize is technical possible but would be a new feature.

	    return ResizeInfo(false, RB_EXTENDED_PARTITION);
	}

	ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info(get_non_impl());

	// minimal size is one sector

	resize_info.combine_min(get_region().get_block_size());

	if (get_region().get_length() <= 1)
	    resize_info.reasons |= RB_MIN_SIZE_FOR_PARTITION;

	// maximal size is limited by used space behind partition

	Region surrounding = get_unused_surrounding_region();

	unsigned long long unused_sectors_behind_partition = get_region().get_length() +
	    surrounding.get_end() - get_region().get_end();

	resize_info.combine_max(surrounding.to_bytes(unused_sectors_behind_partition));

	if (get_region().get_end() >= surrounding.get_end())
	    resize_info.reasons |= RB_NO_SPACE_BEHIND_PARTITION;

	resize_info.combine_block_size(get_region().get_block_size());

	return resize_info;
    }


    RemoveInfo
    Partition::Impl::detect_remove_info() const
    {
	const PartitionTable* partition_table = get_partition_table();

	if ((is_msdos(partition_table) && get_type() == PartitionType::LOGICAL) ||
	    (is_dasd(partition_table)))
	{
	    for (const Partition* partition : partition_table->get_partitions())
	    {
		if (partition->get_number() <= get_number())
		    continue;

		if (partition->get_impl().has_any_active_descendants())
		{
		    return RemoveInfo(false, RMB_RENUMBER_ACTIVE_PARTITIONS);
		}
	    }
	}

	if (is_implicit_pt(partition_table))
	{
	    return RemoveInfo(false, RMB_ON_IMPLICIT_PARTITION_TABLE);
	}

	return RemoveInfo(true, 0);
    }


    Region
    Partition::Impl::get_unused_surrounding_region() const
    {
	const PartitionTable* partition_table = get_partition_table();
	vector<const Partition*> partitions = partition_table->get_partitions();

	switch (get_type())
	{
	    case PartitionType::PRIMARY:
	    case PartitionType::EXTENDED:
	    {
		if (!partition_table->get_impl().has_usable_region())
		    return get_region();

		Region tmp = partition_table->get_impl().get_usable_region();

		// None partition should start before the minimal MBR gap (by default, 1 MiB
		// in MSDOS partition tables). But, in some cases, the first partition starts
		// before 1 MiB (e.g., partitions for old Windows systems). For this reason,
		// the start sector of the partition is considered when calculating the
		// surrounding region.
		unsigned long long start = min(get_region().get_start(), tmp.get_start());
		unsigned long long end = tmp.get_end();

		for (const Partition* partition : partitions)
		{
		    if (partition->get_type() == PartitionType::PRIMARY ||
			partition->get_type() == PartitionType::EXTENDED)
		    {
			if (partition->get_region().get_end() < get_region().get_start())
			    start = max(start, partition->get_region().get_end() + 1);

			if (partition->get_region().get_start() > get_region().get_end())
			    end = min(end, partition->get_region().get_start() - 1);
		    }
		}

		return Region(start, end - start + 1, tmp.get_block_size());
	    }

	    case PartitionType::LOGICAL:
	    {
		Region tmp = partition_table->get_extended()->get_region();

		unsigned long long start = tmp.get_start();
		unsigned long long end = tmp.get_end();

		for (const Partition* partition : partitions)
		{
		    if (partition->get_type() == PartitionType::LOGICAL)
		    {
			if (partition->get_region().get_end() < get_region().get_start())
			    start = max(start, partition->get_region().get_end() + 1);

			if (partition->get_region().get_start() > get_region().get_end())
			    end = min(end, partition->get_region().get_start() - 1);
		    }
		}

		// Keep space for EBRs.

		start += Msdos::Impl::num_ebrs;

		return Region(start, end - start + 1, tmp.get_block_size());
	    }
	}

	ST_THROW(Exception("illegal partition type"));
    }


    Text
    Partition::Impl::do_create_text(Tense tense) const
    {
	const PartitionTable* partition_table = get_partition_table();

	Text text;

	if (is_implicit_pt(partition_table))
	{
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/dasda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Create implicit partition %1$s (%2$s)"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/dasda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Creating implicit partition %1$s (%2$s)"));
	}
	else if (!is_msdos(partition_table))
	{
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Create partition %1$s (%2$s)"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Creating partition %1$s (%2$s)"));
	}
	else
	{
	    switch (type)
	    {
		case PartitionType::PRIMARY:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Create primary partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Creating primary partition %1$s (%2$s)"));
		    break;

		case PartitionType::EXTENDED:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Create extended partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Creating extended partition %1$s (%2$s)"));
		    break;

		case PartitionType::LOGICAL:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Create logical partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Creating logical partition %1$s (%2$s)"));
		    break;
	    }
	}

	return sformat(text, get_name(), get_size_text());
    }


    namespace
    {

	string
	quote_label(const string& label)
	{
	    // funny syntax (see https://bugzilla.suse.com/show_bug.cgi?id=1023818)

	    string t = label;

	    boost::replace_all(t, "'", "\\'");
	    boost::replace_all(t, "\"", "\\\"");

	    return "\"'" + t + "'\"";
	}

    }


    void
    Partition::Impl::do_create()
    {
	const PartitionTable* partition_table = get_partition_table();
	const Partitionable* partitionable = partition_table->get_partitionable();

	vector<unsigned int> tmps = do_create_calc_hack();

	do_create_pre_hack(tmps);

	// Note: --wipesignatures is not available in upstream parted (2021-07-26).

	string cmd_line = PARTED_BIN " --script --wipesignatures " + quote(partitionable->get_name()) +
	    " unit s mkpart ";

	if (is_msdos(partition_table))
	    cmd_line += toString(get_type()) + " ";

	if (is_gpt(partition_table))
	    // pass empty string as partition name
	    cmd_line += quote_label("") + " ";

	if (get_type() != PartitionType::EXTENDED)
	{
	    // Telling parted the filesystem type sets the correct partition
	    // id in some cases. Used in add_create_actions() to avoid
	    // redundant actions.
	    //
	    // Note that "fat16" filesystem was also used to set ID_DOS16 id. But parted
	    // is not assigning the correct id anymore. It sets id 0x0e instead of 0x06.

	    switch (get_id())
	    {
		case ID_SWAP:
		    cmd_line += "linux-swap ";
		    break;

		case ID_DOS32:
		    cmd_line += "fat32 ";
		    break;

		case ID_NTFS:
		case ID_WINDOWS_BASIC_DATA:
		    cmd_line += "ntfs ";
		    break;

		default:
		    cmd_line += "ext2 ";
		    break;
	    }
	}

	unsigned long long factor = parted_sector_adjustment_factor();

	cmd_line += to_string(get_region().get_start() * factor) + " " +
	    to_string((get_region().get_end() - tmps.size()) * factor + (factor - 1));

	SystemCmd(UDEVADM_BIN_SETTLE);

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	do_create_post_hack(tmps);

	if (get_type() == PartitionType::PRIMARY || get_type() == PartitionType::LOGICAL)
	{
	    discard_device();
	}
    }


    vector<unsigned int>
    Partition::Impl::do_create_calc_hack() const
    {
	/*
	 * With parted it is not possible to specify the partition number when creating a
	 * new partition. The first free number is used.
	 *
	 * Since rear wants to keep the partition numbers a hack has to be used: Create
	 * temporary partitions so that the main partition gets the desired number, later
	 * remove those temporary partitions again. The temporary partitions are created
	 * at the end of the main partition since in general that is the only available
	 * space. Thus the main partition must first be created with a slightly smaller
	 * size and later be resized.
	 *
	 * Fails miserable if main partition is very small.
	 *
	 * TODO Improve parted or use something else.
	 */

	y2mil("do_create_calc_hack");

	vector<unsigned int> tmps;

	if (get_type() == PartitionType::PRIMARY || get_type() == PartitionType::EXTENDED)
	{
	    const PartitionTable* partition_table = get_partition_table();

	    vector<unsigned int> numbers;
	    for (const Partition* partition : partition_table->get_partitions())
		numbers.push_back(partition->get_number());

	    unsigned int num = get_number();
	    for (unsigned int i = 1; i < num; ++i)
	    {
		if (!contains(numbers, i))
		    tmps.push_back(i);
	    }
	}

	if (!tmps.empty())
	    y2mil("tmps " << tmps);

	return tmps;
    }


    void
    Partition::Impl::do_create_pre_hack(const vector<unsigned int>& tmps)
    {
	if (tmps.empty())
	    return;

	y2mil("do_create_pre_number_hack begin");

	const PartitionTable* partition_table = get_partition_table();
	const Partitionable* partitionable = partition_table->get_partitionable();

	for (unsigned int i = 0; i < tmps.size(); ++i)
	{
	    string cmd_line = PARTED_BIN " --script --wipesignatures " + quote(partitionable->get_name()) +
		" unit s mkpart ";

	    if (is_msdos(partition_table))
		cmd_line += "primary ";

	    if (is_gpt(partition_table))
		cmd_line += "'\"\"' ";

	    cmd_line += "ext2 ";

	    cmd_line += to_string(get_region().get_end() - i) + " " + to_string(get_region().get_end() - i);

	    SystemCmd(UDEVADM_BIN_SETTLE);

	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
	}

	y2mil("do_create_pre_number_hack end");
    }


    void
    Partition::Impl::do_create_post_hack(const vector<unsigned int>& tmps)
    {
	if (tmps.empty())
	    return;

	y2mil("do_create_post_number_hack begin");

	const Partitionable* partitionable = get_partitionable();

	for (unsigned int i : tmps)
	{
	    string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) + " rm " +
		to_string(i);

	    SystemCmd(UDEVADM_BIN_SETTLE);

	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
	}

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) +
	    " unit s resizepart " + to_string(get_number()) + " " + to_string(get_region().get_end());

	SystemCmd(UDEVADM_BIN_SETTLE);

	wait_for_devices({ get_non_impl() });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	y2mil("do_create_post_number_hack end");
    }


    void
    Partition::Impl::do_create_post_verify() const
    {
	// log some data about the partition that might be useful for debugging

	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) +
	    " unit s print";

	SystemCmd cmd(cmd_line, SystemCmd::NoThrow);
    }


    Text
    Partition::Impl::do_set_id_text(Tense tense) const
    {
	const PartitionTable* partition_table = get_partition_table();

	Text tmp = id_to_text(get_id());

	if (partition_table->get_impl().are_partition_id_values_standardized())
	{
	    if (tmp.empty())
	    {
		Text text = tenser(tense,
				   // TRANSLATORS: displayed before action,
				   // %1$s is replaced by partition name (e.g. /dev/sda1),
				   // 0x%2$02X is replaced by partition id (e.g. 0x8E)
				   _("Set id of partition %1$s to 0x%2$02X"),
				   // TRANSLATORS: displayed during action,
				   // %1$s is replaced by partition name (e.g. /dev/sda1),
				   // 0x%2$02X is replaced by partition id (e.g. 0x8E)
				   _("Setting id of partition %1$s to 0x%2$02X"));
		return sformat(text, get_name(), get_id());
	    }
	    else
	    {
		Text text = tenser(tense,
				   // TRANSLATORS: displayed before action,
				   // %1$s is replaced by partition name (e.g. /dev/sda1),
				   // %2$s is replaced by partition id string (e.g. Linux LVM),
				   // 0x%3$02X is replaced by partition id (e.g. 0x8E)
				   _("Set id of partition %1$s to %2$s (0x%3$02X)"),
				   // TRANSLATORS: displayed during action,
				   // %1$s is replaced by partition name (e.g. /dev/sda1),
				   // %2$s is replaced by partition id string (e.g. Linux LVM),
				   // 0x%3$02X is replaced by partition id (e.g. 0x8E)
				   _("Setting id of partition %1$s to %2$s (0x%3$02X)"));
		return sformat(text, get_name(), tmp, get_id());
	    }
	}
	else
	{
	    if (tmp.empty())
		ST_THROW(Exception(sformat("partition id %d of %s does not have text representation",
					   get_id(), get_name())));

	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition id string (e.g. Linux LVM)
			       _("Set id of partition %1$s to %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition id string (e.g. Linux LVM)
			       _("Setting id of partition %1$s to %2$s"));
	    return sformat(text, get_name(), tmp);
	}
    }


    void
    Partition::Impl::do_set_id() const
    {
	const PartitionTable* partition_table = get_partition_table();
	const Partitionable* partitionable = partition_table->get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name());

	// Note: Neither the 'type' option nor the "type-id" command are available in
	// upstream parted (2022-04-22). 'swap' is not available for MS-DOS in upstream
	// parted (2021-07-26). 'swap' for DASD is SUSE specific (2021-10-07).

	map<unsigned int, const char*>::const_iterator it = Parted::id_to_name.find(get_id());
	if (it != Parted::id_to_name.end() && it->first != ID_SWAP)
	{
	    cmd_line += " set " + to_string(get_number()) + " " + string(it->second) + " on";
	}
	else if (is_msdos(partition_table))
	{
	    if (PartedVersion::supports_type_command())
		cmd_line += " type " + to_string(get_number()) + " " + sformat("0x%02x", get_id());
	    else
		cmd_line += " set " + to_string(get_number()) + " type " + to_string(get_id());
	}
	else
	{
	    switch (get_id())
	    {
		case ID_LINUX:
		    // This is tricky but parted has no clearer way - it also fails if the
		    // partition has a swap signature. TODO Use new upstream type command.
		    cmd_line += " set " + to_string(get_number()) + " lvm on set " +
			to_string(get_number()) + " lvm off";
		    break;

		case ID_SWAP:
		    cmd_line += " set " + to_string(get_number()) + " swap on";
		    break;

		default:
		    ST_THROW(Exception("impossible to set partition id"));
		    break;
	    }
	}

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Partition::Impl::do_set_label_text(Tense tense) const
    {
	if (label.empty())
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       _("Clear label of partition %1$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       _("Clearing label of partition %1$s"));

	    return sformat(text, get_name());
	}
	else
	{
	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition label (e.g. ROOT),
			       _("Set label of partition %1$s to %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition label (e.g. ROOT),
			       _("Setting label of partition %1$s to %2$s"));

	    return sformat(text, get_name(), label);
	}
    }


    void
    Partition::Impl::do_set_label() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) + " name " +
	    to_string(get_number()) + " ";

	cmd_line += quote_label(label);

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Partition::Impl::do_set_boot_text(Tense tense) const
    {
	Text text;

	if (is_boot())
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Set boot flag of partition %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Setting boot flag of partition %1$s"));
	else
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Clear boot flag of partition %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Clearing boot flag of partition %1$s"));

	return sformat(text, get_name());
    }


    void
    Partition::Impl::do_set_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " boot " + (is_boot() ? "on" : "off");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Partition::Impl::do_set_legacy_boot_text(Tense tense) const
    {
	Text text;

	if (is_legacy_boot())
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Set legacy boot flag of partition %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Setting legacy boot flag of partition %1$s"));
	else
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Clear legacy boot flag of partition %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1)
			  _("Clearing legacy boot flag of partition %1$s"));

	return sformat(text, get_name());
    }


    void
    Partition::Impl::do_set_legacy_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " legacy_boot " + (is_legacy_boot() ? "on" : "off");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Partition::Impl::do_delete_text(Tense tense) const
    {
	const PartitionTable* partition_table = get_partition_table();

	Text text;

	if (is_implicit_pt(partition_table))
	{
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/dasda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Delete implicit partition %1$s (%2$s)"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/dasda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Deleting implicit partition %1$s (%2$s)"));
	}
	else if (!is_msdos(partition_table))
	{
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Delete partition %1$s (%2$s)"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2.00 GiB)
			  _("Deleting partition %1$s (%2$s)"));
	}
	else
	{
	    switch (type)
	    {
		case PartitionType::PRIMARY:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Delete primary partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Deleting primary partition %1$s (%2$s)"));
		    break;

		case PartitionType::EXTENDED:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Delete extended partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Deleting extended partition %1$s (%2$s)"));
		    break;

		case PartitionType::LOGICAL:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Delete logical partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2.00 GiB)
				  _("Deleting logical partition %1$s (%2$s)"));
		    break;
	    }
	}

	return sformat(text, get_name(), get_size_text());
    }


    void
    Partition::Impl::do_delete() const
    {
	do_delete_efi_boot_mgr();

	if (get_type() == PartitionType::PRIMARY || get_type() == PartitionType::LOGICAL)
	{
	    discard_device();
	}

	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTED_BIN " --script " + quote(partitionable->get_name()) + " rm " +
	    to_string(get_number());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    Partition::Impl::do_delete_efi_boot_mgr() const
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!is_gpt(partition_table))
	    return;

	if (!get_devicegraph()->get_storage()->get_arch().is_efiboot() || !Arch::is_efibootmgr())
	    return;

	const Partitionable* partitionable = partition_table->get_partitionable();

	string cmd_line = EFIBOOTMGR_BIN " --verbose --delete --disk " +
	    quote(partitionable->get_name()) + " --part " + to_string(get_number());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    Text
    Partition::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Partition* partition_lhs = to_partition(action->get_device(commit_data.actiongraph, LHS));
	const Partition* partition_rhs = to_partition(action->get_device(commit_data.actiongraph, RHS));

	Text text;

	switch (action->resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrink partition %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrinking partition %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Grow partition %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Growing partition %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, get_name(), partition_lhs->get_impl().get_size_text(),
		       partition_rhs->get_impl().get_size_text());
    }


    void
    Partition::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Partition* partition_rhs = to_partition(action->get_device(commit_data.actiongraph, RHS));
	const Partitionable* partitionable = get_partitionable();

	// Note: --ignore-busy is not available in upstream parted (2021-07-26).

	string cmd_line = PARTED_BIN " --script --ignore-busy " + quote(partitionable->get_name()) +
	    " unit s resizepart " + to_string(get_number()) + " ";

	unsigned long long factor = parted_sector_adjustment_factor();

	cmd_line += to_string(partition_rhs->get_region().get_end() * factor + (factor - 1));

	wait_for_devices({ get_non_impl() });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    namespace Action
    {

	Text
	SetPartitionId::text(const CommitData& commit_data) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    return partition->get_impl().do_set_id_text(commit_data.tense);
	}


	void
	SetPartitionId::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    partition->get_impl().do_set_id();
	}


	Text
	SetPartitionLabel::text(const CommitData& commit_data) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    return partition->get_impl().do_set_label_text(commit_data.tense);
	}


	void
	SetPartitionLabel::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    partition->get_impl().do_set_label();
	}


	Text
	SetBoot::text(const CommitData& commit_data) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    return partition->get_impl().do_set_boot_text(commit_data.tense);
	}


	void
	SetBoot::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    partition->get_impl().do_set_boot();
	}


	Text
	SetLegacyBoot::text(const CommitData& commit_data) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    return partition->get_impl().do_set_legacy_boot_text(commit_data.tense);
	}


	void
	SetLegacyBoot::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Partition* partition = to_partition(get_device(commit_data.actiongraph, RHS));
	    partition->get_impl().do_set_legacy_boot();
	}

    }


    unsigned int
    Partition::Impl::default_id_for_type(PartitionType type)
    {
	return type == PartitionType::EXTENDED ? ID_EXTENDED : ID_LINUX;
    }


    unsigned long long
    Partition::Impl::parted_sector_adjustment_factor() const
    {
	// See fix_dasd_sector_size() in class Parted, bsc #866535 and bsc #1112037.

	unsigned long long factor = 1;

	if (boost::starts_with(get_name(), DEV_DIR "/dasd"))
	{
	    const PartitionTable* partition_table = get_partition_table();

	    if (is_dasd_pt(partition_table))
	    {
		switch (get_region().get_block_size())
		{
		    case 4096: factor = 8; break;
		    case 2048: factor = 4; break;
		    case 1024: factor = 2; break;
		}
	    }
	}

	if (factor != 1)
	    y2mil("adjusting parted sector by factor:" << factor);

	return factor;
    }


    Text
    id_to_text(unsigned int id)
    {
	// For every id used on GPT or DASD (where
	// is_partition_id_value_standardized returns false) a text is required
	// since it is used in do_set_id_text().

	switch (id)
	{
	    case ID_DOS12:
		// TRANSLATORS: name of partition type
		return _("DOS12 Partition");

	    case ID_DOS16:
		// TRANSLATORS: name of partition type
		return _("DOS16 Partition");

	    case ID_NTFS:
		// TRANSLATORS: name of partition type
		return _("NTFS Partition");

	    case ID_DOS32:
		// TRANSLATORS: name of partition type
		return _("DOS32 Partition");

	    case ID_EXTENDED:
		// TRANSLATORS: name of partition type
		return _("Extended Partition");

	    case ID_DIAG:
		// TRANSLATORS: name of partition type
		return _("Diagnostics Partition");

	    case ID_PREP:
		// TRANSLATORS: name of partition type
		return _("PReP Boot Partition");

	    case ID_SWAP:
		// TRANSLATORS: name of partition type
		return _("Linux Swap");

	    case ID_LINUX:
		// TRANSLATORS: name of partition type
		return _("Linux");

	    case ID_IRST:
		// TRANSLATORS: name of partition type
		return _("Intel RST");

	    case ID_LVM:
		// TRANSLATORS: name of partition type
		return _("Linux LVM");

	    case ID_RAID:
		// TRANSLATORS: name of partition type
		return _("Linux RAID");

	    case ID_ESP:
		// TRANSLATORS: name of partition type
		return _("EFI System Partition");

	    case ID_BIOS_BOOT:
		// TRANSLATORS: name of partition type
		return _("BIOS Boot Partition");

	    case ID_WINDOWS_BASIC_DATA:
		// TRANSLATORS: name of partition type
		return _("Windows Data Partition");

	    case ID_MICROSOFT_RESERVED:
		// TRANSLATORS: name of partition type
		return _("Microsoft Reserved Partition");

	    case ID_LINUX_HOME:
		// TRANSLATORS: name of partition type
		return _("Linux Home");
	}

	return Text();
    }

}

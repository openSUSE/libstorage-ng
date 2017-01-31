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


#include <iostream>

#include "storage/Utils/AppUtil.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Devices/PartitionImpl.h"
#include "storage/Devices/PartitionTableImpl.h"
#include "storage/Devices/Msdos.h"
#include "storage/Devices/DiskImpl.h"
#include "storage/Filesystems/FilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/FreeInfo.h"
#include "storage/Utils/XmlFile.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Partition>::classname = "Partition";


    // strings must match what parted understands
    const vector<string> EnumTraits<PartitionType>::names({
	"primary", "extended", "logical"
    });


    Partition::Impl::Impl(const string& name, const Region& region, PartitionType type)
	: BlkDevice::Impl(name, region), type(type), id(default_id_for_type(type)), boot(false),
	  legacy_boot(false)
    {
    }


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), type(PartitionType::PRIMARY), id(ID_LINUX), boot(false),
	  legacy_boot(false)
    {
	string tmp;

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, PartitionType::PRIMARY);
	getChildValue(node, "id", id);
	getChildValue(node, "boot", boot);
	getChildValue(node, "legacy-boot", legacy_boot);
    }


    void
    Partition::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_1(probed, systeminfo);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = systeminfo.getParted(partitionable->get_name());
	Parted::Entry entry;
	if (!parted.getEntry(get_number(), entry))
	    throw;

	id = entry.id;
	boot = entry.boot;
	legacy_boot = entry.legacy_boot;
    }


    void
    Partition::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "type", toString(type));
	setChildValueIf(node, "id", id, id != 0);
	setChildValueIf(node, "boot", boot, boot);
	setChildValueIf(node, "legacy-boot", legacy_boot, legacy_boot);
    }


    void
    Partition::Impl::check() const
    {
	BlkDevice::Impl::check();

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
	std::pair<string, unsigned int> pair = device_to_name_and_number(get_name());

	set_name(name_and_number_to_device(pair.first, number));

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


    const Partitionable*
    Partition::Impl::get_partitionable() const
    {
	const PartitionTable* partition_table = get_partition_table();

	return partition_table->get_partitionable();
    }


    void
    Partition::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	if (default_id_for_type(type) != id)
	{
	    // For some partition ids it is fine to skip do_set_id() since
	    // do_create() already sets the partition id correctly.

	    static const vector<unsigned int> skip_ids = {
		ID_LINUX, ID_SWAP, ID_DOS16, ID_DOS32, ID_NTFS, ID_WINDOWS_BASIC_DATA
	    };

	    if (!contains(skip_ids, id))
		actions.push_back(new Action::SetPartitionId(get_sid()));
	}

	if (boot)
	    actions.push_back(new Action::SetBoot(get_sid()));

	if (legacy_boot)
	    actions.push_back(new Action::SetLegacyBoot(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Partition::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (get_type() != lhs.get_type())
	{
	    throw runtime_error("cannot change partition type");
	}

	if (get_region().get_start() != lhs.get_region().get_start())
	{
	    throw runtime_error("cannot move partition");
	}

	if (get_id() != lhs.get_id())
	{
	    Action::Base* action = new Action::SetPartitionId(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (boot != lhs.boot)
	{
	    Action::Base* action = new Action::SetBoot(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (legacy_boot != lhs.legacy_boot)
	{
	    Action::Base* action = new Action::SetLegacyBoot(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    void
    Partition::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Partition::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return type == rhs.type && id == rhs.id && boot == rhs.boot &&
	    legacy_boot == rhs.legacy_boot;
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
    }


    void
    Partition::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
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
	    ST_THROW(Exception("illegal partition type"));

	Impl::type = type;
    }


    void
    Partition::Impl::set_id(unsigned int id)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->get_impl().is_partition_id_supported(id))
	    ST_THROW(Exception("illegal partition id"));

	Impl::id = id;
    }


    void
    Partition::Impl::set_boot(bool boot)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->get_impl().is_partition_boot_flag_supported())
	    ST_THROW(Exception("set_boot not supported"));

	if (boot && !Impl::boot)
	{
	    PartitionTable* partition_table = get_partition_table();

	    for (Partition* partition : partition_table->get_partitions())
		partition->get_impl().boot = false;
	}

	Impl::boot = boot;
    }


    void
    Partition::Impl::set_legacy_boot(bool legacy_boot)
    {
	const PartitionTable* partition_table = get_partition_table();

	if (!partition_table->get_impl().is_partition_legacy_boot_flag_supported())
	    ST_THROW(Exception("set_boot not supported"));

	Impl::legacy_boot = legacy_boot;
    }


    void
    Partition::Impl::update_sysfs_name_and_path()
    {
	const Partitionable* partitionable = get_partitionable();

	// TODO different for device-mapper partitions

	if (!partitionable->get_sysfs_name().empty() && !partitionable->get_sysfs_path().empty())
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

	string postfix = "-part" + to_string(get_number());

	vector<string> udev_paths;
	for (const string& udev_path : partitionable->get_udev_paths())
	    udev_paths.push_back(udev_path + postfix);
	set_udev_paths(udev_paths);

	vector<string> udev_ids;
	for (const string& udev_id : partitionable->get_udev_ids())
	    udev_ids.push_back(udev_id + postfix);
	set_udev_ids(udev_ids);
    }


    ResizeInfo
    Partition::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info();

	// TODO min one sector
	// TODO check free space behind partition
	// TODO check limits of partition table

	return resize_info;
    }


    Text
    Partition::Impl::do_create_text(Tense tense) const
    {
	Text text;

	if (!is_msdos(get_partition_table()))
	{
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2GiB)
			  _("Create partition %1$s (%2$s)"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by partition name (e.g. /dev/sda1),
			  // %2$s is replaced by size (e.g. 2GiB)
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
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Create primary partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Creating primary partition %1$s (%2$s)"));
		    break;

		case PartitionType::EXTENDED:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Create extended partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Creating extended partition %1$s (%2$s)"));
		    break;

		case PartitionType::LOGICAL:
		    text = tenser(tense,
				  // TRANSLATORS: displayed before action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Create logical partition %1$s (%2$s)"),
				  // TRANSLATORS: displayed during action,
				  // %1$s is replaced by partition name (e.g. /dev/sda1),
				  // %2$s is replaced by size (e.g. 2GiB)
				  _("Creating logical partition %1$s (%2$s)"));
		    break;
	    }
	}

	return sformat(text, get_name().c_str(), get_size_string().c_str());
    }


    void
    Partition::Impl::do_create() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script --wipesignatures " + quote(partitionable->get_name()) +
	    " unit s mkpart " + toString(get_type()) + " ";

	if (get_type() != PartitionType::EXTENDED)
	{
	    // Telling parted the filesystem type sets the correct partition
	    // id in some cases. Used in add_create_actions() to avoid
	    // redundant actions.

	    switch (get_id())
	    {
		case ID_SWAP:
		    cmd_line += "linux-swap ";
		    break;

		case ID_DOS16:
		    cmd_line += "fat16 ";
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

	cmd_line += to_string(get_region().get_start()) + " " + to_string(get_region().get_end());

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create partition failed"));
    }


    Text
    Partition::Impl::do_set_id_text(Tense tense) const
    {
	const PartitionTable* partition_table = get_partition_table();

	string tmp = id_to_string(get_id());

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
		return sformat(text, get_name().c_str(), get_id());
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
		return sformat(text, get_name().c_str(), tmp.c_str(), get_id());
	    }
	}
	else
	{
	    if (tmp.empty())
		ST_THROW(LogicException("error"));

	    Text text = tenser(tense,
			       // TRANSLATORS: displayed before action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition id string (e.g. Linux LVM)
			       _("Set id of partition %1$s to %2$s"),
			       // TRANSLATORS: displayed during action,
			       // %1$s is replaced by partition name (e.g. /dev/sda1),
			       // %2$s is replaced by partition id string (e.g. Linux LVM)
			       _("Setting id of partition %1$s to %2$s"));
	    return sformat(text, get_name().c_str(), tmp.c_str());
	}
    }


    void
    Partition::Impl::do_set_id() const
    {
	const Partitionable* partitionable = get_partitionable();
	const PartitionTable* partition_table = get_partition_table();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " ";

	if (is_msdos(partition_table))
	{
	    // Note: The type option is not available in upstream parted.

	    cmd_line += "type " + to_string(get_id());
	}
	else
	{
	    switch (get_id())
	    {
		case ID_LINUX:
		    // this is tricky but parted has no clearer way
		    cmd_line += "lvm off";
		    break;

		case ID_SWAP:
		    cmd_line += "swap on";
		    // TODO improve parted
		    ST_THROW(Exception("set swap not supported by parted"));
		    break;

		case ID_LVM:
		    cmd_line += "lvm on";
		    break;

		case ID_RAID:
		    cmd_line += "raid on";
		    break;

		case ID_ESP:
		    cmd_line += "esp on";
		    break;

		case ID_BIOS_BOOT:
		    cmd_line += "bios_grub on";
		    break;

		case ID_PREP:
		    cmd_line += "prep on";
		    break;

		case ID_WINDOWS_BASIC_DATA:
		    cmd_line += "msftdata";
		    break;

		case ID_MICROSOFT_RESERVED:
		    cmd_line += "msftres";
		    break;
	    }
	}

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set partition id failed"));
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

	return sformat(text, get_name().c_str());
    }


    void
    Partition::Impl::do_set_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " boot " + (is_boot() ? "on" : "off");
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set boot failed"));
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

	return sformat(text, get_name().c_str());
    }


    void
    Partition::Impl::do_set_legacy_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " legacy_boot " + (is_legacy_boot() ? "on" : "off");
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set legacy_boot failed"));
    }


    Text
    Partition::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Delete partition %1$s (%2$s)"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1),
			   // %2$s is replaced by size (e.g. 2GiB)
			   _("Deleting partition %1$s (%2$s)"));

	return sformat(text, get_name().c_str(), get_size_string().c_str());
    }


    void
    Partition::Impl::do_delete() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + partitionable->get_name() + " rm " + to_string(get_number());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete partition failed"));
    }


    Text
    Partition::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const
    {
	const Partition* partition_lhs = to_partition(lhs);

	Text text;

	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrink partition %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrinking partition %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Grow partition %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Growing partition %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, get_name().c_str(), partition_lhs->get_size_string().c_str(),
		       get_size_string().c_str());
    }


    void
    Partition::Impl::do_resize(ResizeMode resize_mode) const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " unit s "
	    "resize " + to_string(get_number()) + " " + to_string(get_region().get_end());
	cout << cmd_line << endl;

	wait_for_device();

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize partition failed"));
    }


    namespace Action
    {

	Text
	SetPartitionId::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    return partition->get_impl().do_set_id_text(tense);
	}


	void
	SetPartitionId::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    partition->get_impl().do_set_id();
	}


	Text
	SetBoot::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    return partition->get_impl().do_set_boot_text(tense);
	}


	void
	SetBoot::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    partition->get_impl().do_set_boot();
	}


	Text
	SetLegacyBoot::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    return partition->get_impl().do_set_legacy_boot_text(tense);
	}


	void
	SetLegacyBoot::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Partition* partition = to_partition(get_device_rhs(actiongraph));
	    partition->get_impl().do_set_legacy_boot();
	}

    }


    unsigned int
    Partition::Impl::default_id_for_type(PartitionType type)
    {
	return type == PartitionType::EXTENDED ? ID_EXTENDED : ID_LINUX;
    }


    string
    id_to_string(unsigned int id)
    {
	// For every id used on GPT or DASD (where
	// is_partition_id_value_standardized returns false) a text is required
	// since it is used in do_set_id_text().

	switch (id)
	{
	    case ID_SWAP: return "Linux Swap";
	    case ID_LINUX: return "Linux";
	    case ID_LVM: return "Linux LVM";
	    case ID_RAID: return "Linux RAID";
	    case ID_ESP: return "EFI System Partition";
	    case ID_BIOS_BOOT: return "BIOS Boot Partition";
	    case ID_PREP: return "PReP Boot Partition";
	    case ID_WINDOWS_BASIC_DATA: return "Windows Data Partition";
	    case ID_MICROSOFT_RESERVED: return "Microsoft Reserved Partition";
	}

	return "";
    }


    bool
    compare_by_number(const Partition* lhs, const Partition* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }

}

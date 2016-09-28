/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) 2016 SUSE LLC
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
	: BlkDevice::Impl(name, region), type(type), id(ID_LINUX), boot(false)
    {
    }


    Partition::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), type(PartitionType::PRIMARY), id(ID_LINUX), boot(false)
    {
	string tmp;

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, PartitionType::PRIMARY);
	getChildValue(node, "id", id);
	getChildValue(node, "boot", boot);
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
    }


    void
    Partition::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "type", toString(type));
	setChildValueIf(node, "id", id, id != 0);
	setChildValueIf(node, "boot", boot, boot);
    }


    unsigned int
    Partition::Impl::get_number() const
    {
	string::size_type pos = get_name().find_last_not_of("0123456789");
	if (pos == string::npos || pos == get_name().size() - 1)
	    ST_THROW(Exception("partition name has no number"));

	return atoi(get_name().substr(pos + 1).c_str());
    }


    void
    Partition::Impl::set_region(const Region& region)
    {
	const Region& partitionable_region = get_partitionable()->get_region();
	if (region.get_block_size() != partitionable_region.get_block_size())
	    ST_THROW(DifferentBlockSizes(region.get_block_size(), partitionable_region.get_block_size()));

	BlkDevice::Impl::set_region(region);
    }


    const PartitionTable*
    Partition::Impl::get_partition_table() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_partition_table(get_devicegraph()->get_impl()[vertex]);
    }


    const Partitionable*
    Partition::Impl::get_partitionable() const
    {
	const PartitionTable* partition_table = get_partition_table();

	Devicegraph::Impl::vertex_descriptor vertex =
	    get_devicegraph()->get_impl().parent(partition_table->get_impl().get_vertex());

	return to_partitionable(get_devicegraph()->get_impl()[vertex]);
    }


    void
    Partition::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::SetPartitionId(get_sid()));

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

	return type == rhs.type && id == rhs.id && boot == rhs.boot;
    }


    void
    Partition::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "type", type, rhs.type);
	storage::log_diff_hex(log, "id", id, rhs.id);
	storage::log_diff(log, "boot", boot, rhs.boot);
    }


    void
    Partition::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);
    }


    void
    Partition::Impl::process_udev_ids(vector<string>& udev_ids) const
    {
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().process_udev_ids(udev_ids);
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

	string cmd_line = PARTEDBIN " --script --wipesignatures " + quote(partitionable->get_name()) + " unit s mkpart " +
	    toString(get_type()) + " ";

	if (get_type() != PartitionType::EXTENDED)
	{
	    switch (get_id())
	    {
		case ID_SWAP:
		    cmd_line += "linux-swap ";
		    break;

		case ID_DOS16:
		    cmd_line += "fat16 ";
		    break;

		case ID_GPT_BOOT:
		case ID_DOS32:
		    cmd_line += "fat32 ";
		    break;

		case ID_NTFS:
		    cmd_line += "ntfs ";
		    break;

		case ID_APPLE_HFS:
		    cmd_line += "hfs ";
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
	string tmp = id_to_string(get_id());

	if (tmp.empty())
	    return sformat(_("Set id of partition %1$s to 0x%2$02X"), get_name().c_str(), get_id());
	else
	    return sformat(_("Set id of partition %1$s to %2$s (0x%3$02X)"), get_name().c_str(),
			   tmp.c_str(), get_id());
    }


    void
    Partition::Impl::do_set_id() const
    {
	const Partitionable* partitionable = get_partitionable();
	const PartitionTable* partition_table = get_partition_table();

	// TODO

	if (get_id() > 255 || !is_msdos(partition_table))
	    return;

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " set " +
	    to_string(get_number()) + " type " + to_string(get_id());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set partition id failed"));
    }


    Text
    Partition::Impl::do_delete_text(Tense tense) const
    {
	return sformat(_("Delete partition %1$s (%2$s)"), get_name().c_str(),
		       get_size_string().c_str());
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

    }

    string
    id_to_string(unsigned int id)
    {
	switch (id)
	{
	    case ID_SWAP: return "Linux Swap";
	    case ID_LINUX: return "Linux";
	    case ID_LVM: return "Linux LVM";
	    case ID_RAID: return "Linux RAID";
	}

	return "";
    }


    bool
    compare_by_number(const Partition* lhs, const Partition* rhs)
    {
	return lhs->get_number() < rhs->get_number();
    }

}

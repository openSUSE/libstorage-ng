/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2023] SUSE LLC
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
#include "storage/Devices/GptImpl.h"
#include "storage/Devices/PartitionableImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/SystemInfo/CmdParted.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"
#include "storage/Actions/SetPmbrBootImpl.h"
#include "storage/Actions/RepairImpl.h"
#include "storage/Actions/CreateImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Gpt>::classname = "Gpt";

    const unsigned int Gpt::Impl::default_partition_slots;


    Gpt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node), partition_slots(default_partition_slots), undersized(false),
	  backup_broken(false), pmbr_boot(false)
    {
	getChildValue(node, "partition-slots", partition_slots);

	getChildValue(node, "undersized", undersized);
	getChildValue(node, "backup-broken", backup_broken);

	getChildValue(node, "pmbr-boot", pmbr_boot);
    }


    string
    Gpt::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("GPT").translated;
    }


    void
    Gpt::Impl::probe_pass_1c(Prober& prober)
    {
	PartitionTable::Impl::probe_pass_1c(prober);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = prober.get_system_info().getParted(partitionable->get_name());

	if (parted.get_primary_slots() >= 0)
	    partition_slots = parted.get_primary_slots();

	undersized = parted.is_gpt_undersized();
	backup_broken = parted.is_gpt_backup_broken();

	pmbr_boot = parted.is_gpt_pmbr_boot();
    }


    void
    Gpt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "partition-slots", partition_slots, partition_slots != default_partition_slots);

	setChildValueIf(node, "undersized", undersized, undersized);
	setChildValueIf(node, "backup-broken", backup_broken, backup_broken);

	setChildValueIf(node, "pmbr-boot", pmbr_boot, pmbr_boot);
    }


    pair<unsigned long long, unsigned long long>
    Gpt::Impl::unusable_sectors() const
    {
	Region region = get_partitionable()->get_region();

	// 1 sector for protective MBR (only at beginning), 1 sector
	// for primary or secondary header and partition_slots,
	// usually 128, partition entries with 128 bytes per partition
	// entry. In theory these values are variables. See
	// https://en.wikipedia.org/wiki/GUID_Partition_Table.

	unsigned long long sectors = 1 + partition_slots * 128 * B / region.get_block_size();

	return make_pair(1 + sectors, sectors);
    }


    void
    Gpt::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<shared_ptr<Action::Base>> actions;

	actions.push_back(make_shared<Action::Create>(get_sid()));

	if (pmbr_boot)
	    actions.push_back(make_shared<Action::SetPmbrBoot>(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Gpt::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	PartitionTable::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (pmbr_boot != lhs.pmbr_boot)
	{
	    shared_ptr<Action::SetPmbrBoot> action = make_shared<Action::SetPmbrBoot>(get_sid());
	    actiongraph.add_vertex(action);
	}

	if (!undersized && lhs.undersized)
	{
	    shared_ptr<Action::Repair> action = make_shared<Action::Repair>(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    bool
    Gpt::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return partition_slots == rhs.partition_slots && undersized == rhs.undersized &&
	    backup_broken == rhs.backup_broken && pmbr_boot == rhs.pmbr_boot;
    }


    void
    Gpt::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "partition-slots", partition_slots, rhs.partition_slots);

	storage::log_diff(log, "undersized", undersized, rhs.undersized);
	storage::log_diff(log, "backup-broken", backup_broken, rhs.backup_broken);

	storage::log_diff(log, "pmbr-boot", pmbr_boot, rhs.pmbr_boot);
    }


    void
    Gpt::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);

	if (partition_slots != default_partition_slots)
	    out << " partition-slots:" << partition_slots;

	if (is_undersized())
	    out << " undersized";

	if (is_backup_broken())
	    out << " backup-broken";

	if (is_pmbr_boot())
	    out << " pmbr-boot";
    }


    bool
    Gpt::Impl::is_partition_type_supported(PartitionType type) const
    {
	return type == PartitionType::PRIMARY;
    }


    bool
    Gpt::Impl::is_partition_id_supported(unsigned int id) const
    {
	static const vector<unsigned int> supported_ids = {
	    ID_UNKNOWN, ID_LINUX, ID_SWAP, ID_LVM, ID_RAID, ID_IRST, ID_ESP, ID_BIOS_BOOT,
	    ID_PREP, ID_WINDOWS_BASIC_DATA, ID_MICROSOFT_RESERVED, ID_DIAG
	};

	if (contains(supported_ids, id))
	    return true;

	// For more ids the type command of parted 3.6 is needed.

	if (!CmdPartedVersion::supports_type_command())
	    return false;

	static const vector<unsigned int> more_supported_ids = {
	    ID_LINUX_HOME, ID_LINUX_SERVER_DATA, ID_LINUX_ROOT_ARM, ID_LINUX_ROOT_AARCH64,
	    ID_LINUX_ROOT_PPC32, ID_LINUX_ROOT_PPC64BE, ID_LINUX_ROOT_PPC64LE,
	    ID_LINUX_ROOT_RISCV32, ID_LINUX_ROOT_RISCV64, ID_LINUX_ROOT_S390,
	    ID_LINUX_ROOT_S390X, ID_LINUX_ROOT_X86, ID_LINUX_ROOT_X86_64,
	    ID_LINUX_USR_ARM, ID_LINUX_USR_AARCH64, ID_LINUX_USR_PPC32,
	    ID_LINUX_USR_PPC64BE, ID_LINUX_USR_PPC64LE, ID_LINUX_USR_RISCV32,
	    ID_LINUX_USR_RISCV64, ID_LINUX_USR_S390, ID_LINUX_USR_S390X, ID_LINUX_USR_X86,
	    ID_LINUX_USR_X86_64
	};

	return contains(more_supported_ids, id);
    }


    bool
    Gpt::Impl::is_partition_no_automount_flag_supported() const
    {
	return CmdPartedVersion::supports_no_automount_flag();
    }


    unsigned int
    Gpt::Impl::max_primary() const
    {
	return min(partition_slots, get_partitionable()->get_range() - 1);
    }


    Text
    Gpt::Impl::do_create_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Create GPT on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Creating GPT on %1$s"));

	return sformat(text, partitionable->get_displayname());
    }


    void
    Gpt::Impl::do_create()
    {
	const Partitionable* partitionable = get_partitionable();

	partitionable->get_impl().discard_device();

	SystemCmd::Args cmd_args = { PARTED_BIN, "--script", partitionable->get_name(), "mklabel", "gpt" };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);

	udev_settle();
    }


    Text
    Gpt::Impl::do_repair_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Repair GPT on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Repairing GPT on %1$s"));

	return sformat(text, partitionable->get_displayname());
    }


    void
    Gpt::Impl::do_repair() const
    {
	// The interface to parted to make a GPT use the complete device is
	// very clumsy. Depending on whether the backup GPT is also broken we
	// need to answer different questions from parted. Improvements are
	// not in sight (see bsc #946673).

	// Note: If only the backup GPT is broken parted has no troubles in
	// script mode so no repair is needed (and this action must not be
	// called).

	const Partitionable* partitionable = get_partitionable();

	y2mil("undersized:" << undersized << " backup-broken:" << backup_broken);

	string answers = backup_broken ? "'OK\\nFix\\n'" : "'Fix\\n'";

	string cmd_line(ECHO_BIN " -e -n " + answers + " | " PARTED_BIN " ---pretend-input-tty " +
			quote(partitionable->get_name()) + " print");

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	udev_settle();
    }


    Text
    Gpt::Impl::do_set_pmbr_boot_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text;

	if (is_pmbr_boot())
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by device name (e.g. /dev/sda)
			  _("Set protective MBR boot flag of GPT on %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by device name (e.g. /dev/sda)
			  _("Setting protective MBR boot flag of GPT on %1$s"));
	else
	    text = tenser(tense,
			  // TRANSLATORS: displayed before action,
			  // %1$s is replaced by device name (e.g. /dev/sda)
			  _("Clear protective MBR boot flag of GPT on %1$s"),
			  // TRANSLATORS: displayed during action,
			  // %1$s is replaced by device name (e.g. /dev/sda)
			  _("Clearing protective MBR boot flag of GPT on %1$s"));

	return sformat(text, partitionable->get_name());
    }


    void
    Gpt::Impl::do_set_pmbr_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	SystemCmd::Args cmd_args = { PARTED_BIN, "--script", partitionable->get_name(),
	    "disk_set", "pmbr_boot", is_pmbr_boot() ? "on" : "off" };

	SystemCmd cmd(cmd_args, SystemCmd::DoThrow);
    }


    Text
    Gpt::Impl::do_delete_text(Tense tense) const
    {
	const Partitionable* partitionable = get_partitionable();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Delete GPT on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda)
			   _("Deleting GPT on %1$s"));

	return sformat(text, partitionable->get_displayname());
    }

}

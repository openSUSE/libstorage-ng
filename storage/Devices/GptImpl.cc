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

#include "storage/Utils/HumanString.h"
#include "storage/Devices/GptImpl.h"
#include "storage/Devices/Partitionable.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/Utils/XmlFile.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Gpt>::classname = "Gpt";


    Gpt::Impl::Impl(const xmlNode* node)
	: PartitionTable::Impl(node), enlarge(false), pmbr_boot(false)
    {
	getChildValue(node, "enlarge", enlarge);
	getChildValue(node, "pmbr-boot", pmbr_boot);
    }


    void
    Gpt::Impl::probe_pass_1c(Prober& prober)
    {
	PartitionTable::Impl::probe_pass_1c(prober);

	const Partitionable* partitionable = get_partitionable();

	const Parted& parted = prober.get_system_info().getParted(partitionable->get_name());

	enlarge = parted.is_gpt_enlarge();
	pmbr_boot = parted.is_gpt_pmbr_boot();
    }


    void
    Gpt::Impl::save(xmlNode* node) const
    {
	PartitionTable::Impl::save(node);

	setChildValueIf(node, "enlarge", enlarge, enlarge);
	setChildValueIf(node, "pmbr-boot", pmbr_boot, pmbr_boot);
    }


    Region
    Gpt::Impl::get_usable_region() const
    {
	Region device_region = get_partitionable()->get_region();

	// 1 sector for protective MBR (only at beginning), 1 sector for
	// primary or secondary header and 128 partition entries with 128
	// bytes per partition entry. In theory these values are
	// variables. See https://en.wikipedia.org/wiki/GUID_Partition_Table.

	unsigned long long sectors = 1 + 128 * 128 * B / device_region.get_block_size();

	unsigned long long first_usable_sector = 1 + sectors;
	unsigned long long last_usable_sector = device_region.get_end() - sectors;
	Region usable_region(first_usable_sector, last_usable_sector - first_usable_sector + 1,
			     device_region.get_block_size());

	return device_region.intersection(usable_region);
    }


    void
    Gpt::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));

	if (pmbr_boot)
	    actions.push_back(new Action::SetPmbrBoot(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Gpt::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	PartitionTable::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (pmbr_boot != lhs.pmbr_boot)
	{
	    Action::Base* action = new Action::SetPmbrBoot(get_sid());
	    actiongraph.add_vertex(action);
	}
    }


    bool
    Gpt::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!PartitionTable::Impl::equal(rhs))
	    return false;

	return enlarge == rhs.enlarge && pmbr_boot == rhs.pmbr_boot;
    }


    void
    Gpt::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	PartitionTable::Impl::log_diff(log, rhs);

	storage::log_diff(log, "enlarge", enlarge, rhs.enlarge);
	storage::log_diff(log, "pmbr-boot", pmbr_boot, rhs.pmbr_boot);
    }


    void
    Gpt::Impl::print(std::ostream& out) const
    {
	PartitionTable::Impl::print(out);

	if (is_enlarge())
	    out << " enlarge";

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
	    ID_UNKNOWN, ID_LINUX, ID_SWAP, ID_LVM, ID_RAID, ID_ESP, ID_BIOS_BOOT, ID_PREP,
	    ID_WINDOWS_BASIC_DATA, ID_MICROSOFT_RESERVED
	};

	return contains(supported_ids, id);
    }


    unsigned int
    Gpt::Impl::max_primary() const
    {
	return min(128U, get_partitionable()->get_range());
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

	return sformat(text, partitionable->get_displayname().c_str());
    }


    void
    Gpt::Impl::do_create()
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) + " mklabel gpt";
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create gpt failed"));

	SystemCmd(UDEVADMBIN_SETTLE);
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

	return sformat(text, partitionable->get_name().c_str());
    }


    void
    Gpt::Impl::do_set_pmbr_boot() const
    {
	const Partitionable* partitionable = get_partitionable();

	string cmd_line = PARTEDBIN " --script " + quote(partitionable->get_name()) +
	    " disk_set pmbr_boot " + (is_pmbr_boot() ? "on" : "off");
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("set pmbr boot failed"));
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

	return sformat(text, partitionable->get_displayname().c_str());
    }


    namespace Action
    {

	Text
	SetPmbrBoot::text(const CommitData& commit_data) const
	{
	    const Gpt* gpt = to_gpt(get_device(commit_data.actiongraph, RHS));
	    return gpt->get_impl().do_set_pmbr_boot_text(commit_data.tense);
	}


	void
	SetPmbrBoot::commit(CommitData& commit_data) const
	{
	    const Gpt* gpt = to_gpt(get_device(commit_data.actiongraph, RHS));
	    gpt->get_impl().do_set_pmbr_boot();
	}

    }

}

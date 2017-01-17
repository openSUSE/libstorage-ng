/*
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


#include "storage/Utils/XmlFile.h"
#include "storage/Utils/StorageTmpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/StorageImpl.h"
#include "storage/EnvironmentImpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Encryption>::classname = "Encryption";


    const vector<string> EnumTraits<EncryptionType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks", "unknown"
    });


    Encryption::Impl::Impl(const string& dm_table_name)
	: BlkDevice::Impl(DEVDIR "/mapper/" + dm_table_name)
    {
	set_dm_table_name(dm_table_name);
    }


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
	if (get_dm_table_name().empty())
	    ST_THROW(Exception("no dm-table-name"));

	getChildValue(node, "password", password);
    }


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    setChildValue(node, "password", password);
    }


    const BlkDevice*
    Encryption::Impl::get_blk_device() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_blk_device(get_devicegraph()->get_impl()[vertex]);
    }


    void
    Encryption::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	BlkDevice::Impl::probe_pass_2(probed, systeminfo);

	const CmdDmsetupTable& cmd_dmsetup_table = systeminfo.getCmdDmsetupTable();

	vector<CmdDmsetupTable::Table> tables = cmd_dmsetup_table.get_tables(get_dm_table_name());

	if (tables.size() != 1)
	    ST_THROW(Exception("crypt with several dm-tables"));

	const vector<dev_t>& majorminors = tables.front().majorminors;

	if (majorminors.size() != 1)
	    ST_THROW(Exception("crypt target with several devices"));

	BlkDevice* blk_device = BlkDevice::Impl::find_by_name(probed,
	    make_dev_block_name(majorminors.front()), systeminfo);

	User::create(probed, blk_device, get_device());
    }


    void
    Encryption::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::Activate(get_sid()));
	actions.push_back(new Action::AddEtcCrypttab(get_sid()));

	actiongraph.add_chain(actions);
    }


    void
    Encryption::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());
	vector<Action::Base*> actions;

	// TODO depends on mount-by, whether there actually is an entry in crypttab
	if (get_blk_device()->get_name() != lhs.get_blk_device()->get_name())
	{
	    actions.push_back(new Action::RenameEtcCrypttab(get_sid()));
	}

	actiongraph.add_chain(actions);
    }


    void
    Encryption::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::RemoveEtcCrypttab(get_sid()));
	actions.push_back(new Action::Deactivate(get_sid()));
	actions.push_back(new Action::Delete(get_sid()));

	actiongraph.add_chain(actions);
    }


    bool
    Encryption::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!BlkDevice::Impl::equal(rhs))
	    return false;

	return password == rhs.password;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    storage::log_diff(log, "password", password, rhs.password);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    out << " password:" << get_password();
    }


    Text
    Encryption::Impl::do_create_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Create encryption layer device on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Creating encryption layer device on %1$s"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_delete_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Delete encryption layer device on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Deleting encryption layer device on %1$s"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, Tense tense) const
    {
	const Encryption* encryption_lhs = to_encryption(lhs);

	Text text;

	switch (resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrink encryption layer device on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2GiB),
			      // %3$s is replaced by new size (e.g. 1GiB)
			      _("Shrinking encryption layer device on %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Grow encryption layer device on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1GiB),
			      // %3$s is replaced by new size (e.g. 2GiB)
			      _("Growing encryption layer device on %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, get_name().c_str(), encryption_lhs->get_size_string().c_str(),
		       get_size_string().c_str());
    }


    Text
    Encryption::Impl::do_activate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Activate encryption layer device on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Activating encryption layer device on %1$s"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_deactivate_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Deactivate encryption layer device on %1$s"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Deactivating encryption layer device on %1$s"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    Text
    Encryption::Impl::do_add_etc_crypttab_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Add encryption layer device on %1$s to /etc/crypttab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding encryption layer device on %1$s to /etc/crypttab"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    void
    Encryption::Impl::do_add_etc_crypttab(const Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(LogicException("stub do_add_etc_crypttab called"));
    }


    Text
    Encryption::Impl::do_rename_etc_crypttab_text(const Device* lhs, Tense tense) const
    {
	const BlkDevice* blk_device_lhs = to_encryption(lhs)->get_impl().get_blk_device();
	const BlkDevice* blk_device_rhs = get_blk_device();

	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda6),
			   // %2$s is replaced by device name (e.g. /dev/sda5)
			   _("Rename encryption layer device from %1$s to %2$s in /etc/crypttab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda6),
			   // %2$s is replaced by device name (e.g. /dev/sda5)
			   _("Renaming encryption layer device from %1$s to %2$s in /etc/crypttab"));

	return sformat(text,  blk_device_lhs->get_displayname().c_str(),
		       blk_device_rhs->get_displayname().c_str());
    }


    void
    Encryption::Impl::do_rename_etc_crypttab(const Actiongraph::Impl& actiongraph,
					     const Device* lhs) const
    {
        ST_THROW(LogicException("stub do_rename_etc_crypttab called"));
    }


    Text
    Encryption::Impl::do_remove_etc_crypttab_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove encryption layer device on %1$s from /etc/crypttab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing encryption layer device on %1$s from /etc/crypttab"));

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    void
    Encryption::Impl::do_remove_etc_crypttab(const Actiongraph::Impl& actiongraph) const
    {
	ST_THROW(LogicException("stub do_remove_etc_crypttab called"));
    }


    namespace Action
    {

	Text
	AddEtcCrypttab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Encryption* encryption = to_encryption(get_device_rhs(actiongraph));
	    return encryption->get_impl().do_add_etc_crypttab_text(tense);
	}


	void
	AddEtcCrypttab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Encryption* encryption = to_encryption(get_device_rhs(actiongraph));
	    encryption->get_impl().do_add_etc_crypttab(actiongraph);
	}


	void
	AddEtcCrypttab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					 Actiongraph::Impl& actiongraph) const
	{
	    if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}


	Text
	RenameEtcCrypttab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Encryption* encryption_lhs = to_encryption(get_device_lhs(actiongraph));
	    const Encryption* encryption_rhs = to_encryption(get_device_rhs(actiongraph));
	    return encryption_rhs->get_impl().do_rename_etc_crypttab_text(encryption_lhs, tense);
	}


	void
	RenameEtcCrypttab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Encryption* encryption_lhs = to_encryption(get_device_lhs(actiongraph));
	    const Encryption* encryption_rhs = to_encryption(get_device_rhs(actiongraph));
	    encryption_rhs->get_impl().do_rename_etc_crypttab(actiongraph, encryption_lhs);
	}


	Text
	RemoveEtcCrypttab::text(const Actiongraph::Impl& actiongraph, Tense tense) const
	{
	    const Encryption* encryption = to_encryption(get_device_lhs(actiongraph));
	    return encryption->get_impl().do_remove_etc_crypttab_text(tense);
	}


	void
	RemoveEtcCrypttab::commit(const Actiongraph::Impl& actiongraph) const
	{
	    const Encryption* encryption = to_encryption(get_device_lhs(actiongraph));
	    encryption->get_impl().do_remove_etc_crypttab(actiongraph);
	}

    }

}

/*
 * Copyright (c) [2016-2019] SUSE LLC
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
#include "storage/Utils/HumanString.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/Devices/EncryptionImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/StorageImpl.h"
#include "storage/EnvironmentImpl.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Prober.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Encryption>::classname = "Encryption";


    const vector<string> EnumTraits<EncryptionType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks1", "unknown", "luks2", "plain"
    });


    Encryption::Impl::Impl(const string& dm_table_name)
	: BlkDevice::Impl(DEV_MAPPER_DIR "/" + dm_table_name)
    {
	set_dm_table_name(dm_table_name);
    }


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node)
    {
	string tmp;

	if (get_dm_table_name().empty())
	    ST_THROW(Exception("no dm-table-name"));

	if (getChildValue(node, "type", tmp))
	    type = toValueWithFallback(tmp, EncryptionType::UNKNOWN);

	getChildValue(node, "password", password);

	getChildValue(node, "key-file", key_file);

	getChildValue(node, "cipher", cipher);
	getChildValue(node, "key-size", key_size);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "crypt-options", tmp))
	    crypt_options.parse(tmp);

	getChildValue(node, "in-etc-crypttab", in_etc_crypttab);

	getChildValue(node, "open-options", open_options);
    }


    string
    Encryption::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Encryption").translated;
    }


    void
    Encryption::Impl::probe_pass_1a(Prober& prober)
    {
	BlkDevice::Impl::probe_pass_1a(prober);

	if (is_active())
	{
	    probe_size(prober);
	    probe_topology(prober);
	}
    }


    string
    Encryption::Impl::next_free_cr_auto_name(SystemInfo::Impl& system_info)
    {
	const CmdDmsetupInfo& cmd_dmsetup_info = system_info.getCmdDmsetupInfo();
	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab();

	static int nr = 1;

	while (true)
	{
	    string name = "cr-auto-" + to_string(nr++);

	    if (!cmd_dmsetup_info.exists(name) && !etc_crypttab.has_crypt_device(name))
		return name;
	}
    }


    void
    Encryption::Impl::set_default_mount_by()
    {
	set_mount_by(get_storage()->get_default_mount_by());
    }


    void
    Encryption::Impl::set_dm_table_name(const string& dm_table_name)
    {
	BlkDevice::Impl::set_dm_table_name(dm_table_name);
	BlkDevice::Impl::set_name(DEV_MAPPER_DIR "/" + dm_table_name);
    }


    void
    Encryption::Impl::set_crypt_options(const CryptOpts& crypt_options)
    {
	Impl::crypt_options = crypt_options;
    }


    void
    Encryption::Impl::set_crypt_options(const vector<string>& crypt_options)
    {
	Impl::crypt_options.set_opts(crypt_options);
    }


    string
    Encryption::Impl::get_mount_by_name(MountByType mount_by_type) const
    {
	string ret;

	switch (mount_by_type)
	{
	    case MountByType::UUID:
		y2war("no uuid possible, using fallback");
		break;

	    case MountByType::LABEL:
		y2war("no label possible, using fallback");
		break;

	    case MountByType::ID:
	    case MountByType::PATH:
	    case MountByType::DEVICE:
		break;
	}

	if (ret.empty())
	{
	    const BlkDevice* blk_device = get_blk_device();

	    ret = blk_device->get_impl().get_mount_by_name(mount_by_type);
	}

	return ret;
    }


    void
    Encryption::Impl::save(xmlNode* node) const
    {
	BlkDevice::Impl::save(node);

	setChildValue(node, "type", toString(type));

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    setChildValue(node, "password", password);

	setChildValueIf(node, "key-file", key_file, !key_file.empty());

	setChildValueIf(node, "cipher", cipher, !cipher.empty());
	setChildValueIf(node, "key-size", key_size, key_size != 0);

	setChildValue(node, "mount-by", toString(mount_by));

	if (!crypt_options.empty())
	    setChildValue(node, "crypt-options", crypt_options.format());

	setChildValue(node, "in-etc-crypttab", in_etc_crypttab);

	setChildValueIf(node, "open-options", open_options, !open_options.empty());
    }


    void
    Encryption::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	BlkDevice::Impl::check(check_callbacks);

	if (!has_single_parent_of_type<const BlkDevice>())
	    ST_THROW(Exception("Encryption has no BlkDevice parent"));

	if (get_size() > get_blk_device()->get_size())
	    ST_THROW(Exception("Encryption bigger than parent BlkDevice"));
    }


    const BlkDevice*
    Encryption::Impl::get_blk_device() const
    {
	Devicegraph::Impl::vertex_descriptor vertex = get_devicegraph()->get_impl().parent(get_vertex());

	return to_blk_device(get_devicegraph()->get_impl()[vertex]);
    }


    void
    Encryption::Impl::add_create_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	actions.push_back(new Action::Create(get_sid()));
	actions.push_back(new Action::Activate(get_sid()));

	if (in_etc_crypttab)
	    actions.push_back(new Action::AddToEtcCrypttab(get_sid()));

	actiongraph.add_chain(actions);

	// Normally last means that the action is the last for the object. But
	// this fails for LUKS encryption sind adding to /etc/crypttab must
	// happen after the root filesystem is mounted. If the root filesystem
	// in somehow located on the LUKS this results in a cycle in the
	// actiongraph. So set last to the activate action.

	// TODO rename last and first? or better define the meaning somewhere?

	if (in_etc_crypttab)
	{
	    actions[1]->last = true;
	    actions[2]->last = false;
	}
    }


    void
    Encryption::Impl::add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs_base) const
    {
	BlkDevice::Impl::add_modify_actions(actiongraph, lhs_base);

	const Impl& lhs = dynamic_cast<const Impl&>(lhs_base->get_impl());

	if (!lhs.in_etc_crypttab && in_etc_crypttab)
	{
	    Action::Base* action = new Action::AddToEtcCrypttab(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.in_etc_crypttab && !in_etc_crypttab)
	{
	    Action::Base* action = new Action::RemoveFromEtcCrypttab(get_sid());
	    actiongraph.add_vertex(action);
	}
	else if (lhs.in_etc_crypttab && in_etc_crypttab)
	{
	    // TODO depends on mount-by

	    if (get_blk_device()->get_name() != lhs.get_blk_device()->get_name())
	    {
		Action::Base* action = new Action::RenameInEtcCrypttab(get_sid(), get_blk_device());
		actiongraph.add_vertex(action);
	    }
	}
    }


    void
    Encryption::Impl::add_delete_actions(Actiongraph::Impl& actiongraph) const
    {
	vector<Action::Base*> actions;

	if (in_etc_crypttab)
	    actions.push_back(new Action::RemoveFromEtcCrypttab(get_sid()));

	if (is_active())
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

	return type == rhs.type && password == rhs.password && key_file == rhs.key_file &&
	    cipher == rhs.cipher && key_size == rhs.key_size &&
	    mount_by == rhs.mount_by && crypt_options == rhs.crypt_options &&
	    in_etc_crypttab == rhs.in_etc_crypttab && open_options == rhs.open_options;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	storage::log_diff_enum(log, "type", type, rhs.type);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    storage::log_diff(log, "password", password, rhs.password);

	storage::log_diff(log, "key-file", key_file, rhs.key_file);

	storage::log_diff(log, "cipher", cipher, rhs.cipher);
	storage::log_diff(log, "key-size", key_size, rhs.key_size);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "crypt-options", crypt_options.get_opts(), rhs.crypt_options.get_opts());

	storage::log_diff(log, "in-etc-crypttab", in_etc_crypttab, rhs.in_etc_crypttab);

	storage::log_diff(log, "open-options", open_options, rhs.open_options);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	out << " type:" << toString(type);

	if (!password.empty())
	    if (get_storage()->get_environment().get_impl().is_debug_credentials())
		out << " password:" << get_password();

	if (!key_file.empty())
	    out << " key-file:" << get_key_file();

	if (!cipher.empty())
	    out << " cipher:" << cipher;

	if (key_size != 0)
	    out << " key-size:" << key_size;

	out << " mount-by:" << toString(mount_by);

	out << " crypt-options:" << crypt_options.get_opts();

	if (in_etc_crypttab)
	    out << " in-etc-crypttab";

	if (!open_options.empty())
	    out << " open-options:" << open_options;
    }


    void
    Encryption::Impl::add_key_file_option_and_execute(const string& cmd_line) const
    {
	const BlkDevice* blk_device = get_blk_device();

	SystemCmd::Options cmd_options(cmd_line, SystemCmd::DoThrow);

	if (!get_key_file().empty())
	{
	    cmd_options.command += " --key-file " + quote(get_key_file());
	}
	else
	{
	    cmd_options.command += " --key-file -";
	    cmd_options.stdin_text = get_password();
	}

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_options);
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

	return sformat(text, get_blk_device()->get_displayname());
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

	return sformat(text, get_blk_device()->get_displayname());
    }


    Text
    Encryption::Impl::do_resize_text(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Encryption* encryption_lhs = to_encryption(action->get_device(commit_data.actiongraph, LHS));
	const Encryption* encryption_rhs = to_encryption(action->get_device(commit_data.actiongraph, RHS));

	Text text;

	switch (action->resize_mode)
	{
	    case ResizeMode::SHRINK:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrink encryption layer device on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 2.00 GiB),
			      // %3$s is replaced by new size (e.g. 1.00 GiB)
			      _("Shrinking encryption layer device on %1$s from %2$s to %3$s"));
		break;

	    case ResizeMode::GROW:
		text = tenser(commit_data.tense,
			      // TRANSLATORS: displayed before action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Grow encryption layer device on %1$s from %2$s to %3$s"),
			      // TRANSLATORS: displayed during action,
			      // %1$s is replaced by partition name (e.g. /dev/sda1),
			      // %2$s is replaced by old size (e.g. 1.00 GiB),
			      // %3$s is replaced by new size (e.g. 2.00 GiB)
			      _("Growing encryption layer device on %1$s from %2$s to %3$s"));
		break;

	    default:
		ST_THROW(LogicException("invalid value for resize_mode"));
	}

	return sformat(text, get_name(), encryption_lhs->get_impl().get_size_text(),
		       encryption_rhs->get_impl().get_size_text());
    }


    void
    Encryption::Impl::do_resize(const CommitData& commit_data, const Action::Resize* action) const
    {
	const Encryption* encryption_rhs = to_encryption(action->get_device(commit_data.actiongraph, RHS));

	string cmd_line = CRYPTSETUP_BIN " resize " + quote(get_dm_table_name());

	if (action->resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --size " + to_string(encryption_rhs->get_impl().get_size() / (512 * B));

	if (do_resize_needs_password())
	    add_key_file_option_and_execute(cmd_line);
	else
	    SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
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

	return sformat(text, get_blk_device()->get_displayname());
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

	return sformat(text, get_blk_device()->get_displayname());
    }


    Text
    Encryption::Impl::do_add_to_etc_crypttab_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Add encryption layer device on %1$s to /etc/crypttab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Adding encryption layer device on %1$s to /etc/crypttab"));

	return sformat(text, get_blk_device()->get_displayname());
    }


    void
    Encryption::Impl::do_add_to_etc_crypttab(CommitData& commit_data) const
    {
	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

	CrypttabEntry* entry = new CrypttabEntry();
	entry->set_crypt_device(get_dm_table_name());
	entry->set_block_device(get_mount_by_name(get_mount_by()));
	if (!key_file.empty())
	    entry->set_password(get_key_file());
	entry->set_crypt_opts(get_crypt_options());

	etc_crypttab.add(entry);
	etc_crypttab.log();
	etc_crypttab.write();
    }


    Text
    Encryption::Impl::do_rename_in_etc_crypttab_text(const Device* lhs, Tense tense) const
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

	return sformat(text, blk_device_lhs->get_displayname(),
		       blk_device_rhs->get_displayname());
    }


    void
    Encryption::Impl::do_rename_in_etc_crypttab(CommitData& commit_data) const
    {
	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

	CrypttabEntry* entry = etc_crypttab.find_block_device(get_crypttab_blk_device_name());
	if (entry)
	{
	    entry->set_block_device(get_mount_by_name(get_mount_by()));
	    etc_crypttab.log();
	    etc_crypttab.write();
	}
    }


    Text
    Encryption::Impl::do_remove_from_etc_crypttab_text(Tense tense) const
    {
	Text text = tenser(tense,
			   // TRANSLATORS: displayed before action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Remove encryption layer device on %1$s from /etc/crypttab"),
			   // TRANSLATORS: displayed during action,
			   // %1$s is replaced by device name (e.g. /dev/sda1)
			   _("Removing encryption layer device on %1$s from /etc/crypttab"));

	return sformat(text, get_blk_device()->get_displayname());
    }


    void
    Encryption::Impl::do_remove_from_etc_crypttab(CommitData& commit_data) const
    {
	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

	CrypttabEntry* entry = etc_crypttab.find_block_device(get_crypttab_blk_device_name());
	if (entry)
	{
	    etc_crypttab.remove(entry);
	    etc_crypttab.log();
	    etc_crypttab.write();
	}
    }


    namespace Action
    {

	Text
	AddToEtcCrypttab::text(const CommitData& commit_data) const
	{
	    const Encryption* encryption = to_encryption(get_device(commit_data.actiongraph, RHS));
	    return encryption->get_impl().do_add_to_etc_crypttab_text(commit_data.tense);
	}


	void
	AddToEtcCrypttab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Encryption* encryption = to_encryption(get_device(commit_data.actiongraph, RHS));
	    encryption->get_impl().do_add_to_etc_crypttab(commit_data);
	}


	void
	AddToEtcCrypttab::add_dependencies(Actiongraph::Impl::vertex_descriptor vertex,
					   Actiongraph::Impl& actiongraph) const
	{
	    if (actiongraph.mount_root_filesystem != actiongraph.vertices().end())
		actiongraph.add_edge(*actiongraph.mount_root_filesystem, vertex);
	}


	Text
	RenameInEtcCrypttab::text(const CommitData& commit_data) const
	{
	    const Encryption* encryption_lhs = to_encryption(get_device(commit_data.actiongraph, LHS));
	    const Encryption* encryption_rhs = to_encryption(get_device(commit_data.actiongraph, RHS));
	    return encryption_rhs->get_impl().do_rename_in_etc_crypttab_text(encryption_lhs, commit_data.tense);
	}


	void
	RenameInEtcCrypttab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Encryption* encryption = to_encryption(get_device(commit_data.actiongraph, RHS));
	    encryption->get_impl().do_rename_in_etc_crypttab(commit_data);
	}


	Text
	RemoveFromEtcCrypttab::text(const CommitData& commit_data) const
	{
	    const Encryption* encryption = to_encryption(get_device(commit_data.actiongraph, LHS));
	    return encryption->get_impl().do_remove_from_etc_crypttab_text(commit_data.tense);
	}


	void
	RemoveFromEtcCrypttab::commit(CommitData& commit_data, const CommitOptions& commit_options) const
	{
	    const Encryption* encryption = to_encryption(get_device(commit_data.actiongraph, LHS));
	    encryption->get_impl().do_remove_from_etc_crypttab(commit_data);
	}

    }

}

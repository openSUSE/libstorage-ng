/*
 * Copyright (c) [2016-2018] SUSE LLC
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
#include "storage/Utils/Format.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Encryption>::classname = "Encryption";


    const vector<string> EnumTraits<EncryptionType>::names({
	"none", "twofish256", "twofish", "twofishSL92", "luks", "unknown"
    });


    Encryption::Impl::Impl(const string& dm_table_name)
	: BlkDevice::Impl(DEV_MAPPER_DIR "/" + dm_table_name), password(), mount_by(MountByType::DEVICE),
	  in_etc_crypttab(true)
    {
	set_dm_table_name(dm_table_name);
    }


    Encryption::Impl::Impl(const xmlNode* node)
	: BlkDevice::Impl(node), password(), mount_by(MountByType::DEVICE), in_etc_crypttab(true)
    {
	string tmp;

	if (get_dm_table_name().empty())
	    ST_THROW(Exception("no dm-table-name"));

	getChildValue(node, "password", password);

	if (getChildValue(node, "mount-by", tmp))
	    mount_by = toValueWithFallback(tmp, MountByType::DEVICE);

	if (getChildValue(node, "crypt-options", tmp))
	    crypt_options.parse(tmp);

	getChildValue(node, "in-etc-crypttab", in_etc_crypttab);
    }


    string
    Encryption::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Encryption").translated;
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
		y2err("no uuid possible, using fallback");
		break;

	    case MountByType::LABEL:
		y2err("no label possible, using fallback");
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

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    setChildValue(node, "password", password);

	setChildValue(node, "mount-by", toString(mount_by));

	if (!crypt_options.empty())
	    setChildValue(node, "crypt-options", crypt_options.format());

	setChildValue(node, "in-etc-crypttab", in_etc_crypttab);
    }


    ResizeInfo
    Encryption::Impl::detect_resize_info() const
    {
	return BlkDevice::Impl::detect_resize_info();
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
		Action::Base* action = new Action::RenameInEtcCrypttab(get_sid());
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

	return password == rhs.password && mount_by == rhs.mount_by &&
	    crypt_options == rhs.crypt_options && in_etc_crypttab == rhs.in_etc_crypttab;
    }


    void
    Encryption::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	BlkDevice::Impl::log_diff(log, rhs);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    storage::log_diff(log, "password", password, rhs.password);

	storage::log_diff_enum(log, "mount-by", mount_by, rhs.mount_by);

	storage::log_diff(log, "crypt-options", crypt_options.get_opts(), rhs.crypt_options.get_opts());

	storage::log_diff(log, "in-etc-crypttab", in_etc_crypttab, rhs.in_etc_crypttab);
    }


    void
    Encryption::Impl::print(std::ostream& out) const
    {
	BlkDevice::Impl::print(out);

	if (get_storage()->get_environment().get_impl().is_debug_credentials())
	    out << " password:" << get_password();

	out << " mount-by:" << toString(mount_by);

	out << " crypt-options:" << crypt_options.get_opts();

	if (in_etc_crypttab)
	    out << " in-etc-crypttab";
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
    Encryption::Impl::do_resize_text(ResizeMode resize_mode, const Device* lhs, const Device* rhs,
				     Tense tense) const
    {
	const Encryption* encryption_lhs = to_encryption(lhs);
	const Encryption* encryption_rhs = to_encryption(rhs);

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

	return sformat(text, get_name(), encryption_lhs->get_impl().get_size_text(),
		       encryption_rhs->get_impl().get_size_text());
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
    Encryption::Impl::do_add_to_etc_crypttab_text(Tense tense) const
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
    Encryption::Impl::do_add_to_etc_crypttab(CommitData& commit_data) const
    {
	ST_THROW(LogicException("stub do_add_to_etc_crypttab called"));
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

	return sformat(text,  blk_device_lhs->get_displayname().c_str(),
		       blk_device_rhs->get_displayname().c_str());
    }


    void
    Encryption::Impl::do_rename_in_etc_crypttab(CommitData& commit_data) const
    {
        ST_THROW(LogicException("stub do_rename_in_etc_crypttab called"));
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

	return sformat(text, get_blk_device()->get_displayname().c_str());
    }


    void
    Encryption::Impl::do_remove_from_etc_crypttab(CommitData& commit_data) const
    {
	ST_THROW(LogicException("stub do_remove_from_etc_crypttab called"));
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


	const BlkDevice*
	RenameInEtcCrypttab::get_renamed_blk_device(const Actiongraph::Impl& actiongraph, Side side) const
	{
	    const Encryption* encryption = to_encryption(get_device(actiongraph, side));
	    const BlkDevice* blk_device = encryption->get_blk_device();
	    return blk_device;
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

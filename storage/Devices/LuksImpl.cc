/*
 * Copyright (c) [2016-2022] SUSE LLC
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

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/CallbacksImpl.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/StorageImpl.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/EtcCrypttab.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"
#include "storage/Utils/StorageTmpl.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Luks>::classname = "Luks";


    Luks::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node)
    {
	getChildValue(node, "uuid", uuid);
	getChildValue(node, "label", label);

	getChildValue(node, "format-options", format_options);
    }


    string
    Luks::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("LUKS").translated;
    }


    void
    Luks::Impl::save(xmlNode* node) const
    {
	Encryption::Impl::save(node);

	setChildValue(node, "uuid", uuid);
	setChildValueIf(node, "label", label, !label.empty());

	setChildValueIf(node, "format-options", format_options, !format_options.empty());
    }


    void
    Luks::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Encryption::Impl::check(check_callbacks);

	if (get_type() != EncryptionType::LUKS1 && get_type() != EncryptionType::LUKS2)
	    ST_THROW(Exception("invalid encryption type for Luks"));

	if (get_cipher() == "aegis128-random" || !get_integrity().empty())
	{
	    if (get_cipher() != "aegis128-random")
		ST_THROW(Exception("when integrity is set it is expected aegis128-random cipher"));

	    if (get_key_size() * 8 != 128)
		ST_THROW(Exception("for aegis128-random it is expected a key-size of 16 (128 bits)"));

	    if (get_integrity() != "aead")
		ST_THROW(Exception("for now only AEAD integrity is supported"));
	}
    }


    void
    Luks::Impl::set_type(EncryptionType type)
    {
	Encryption::Impl::set_type(type);

	// Since the metadata size depends on the type a recalculation
	// is needed.

	calculate_region_and_topology();
    }


    void
    Luks::Impl::set_integrity(const string& integrity)
    {
	Encryption::Impl::set_integrity(integrity);

	calculate_region_and_topology();
    }


    string
    Luks::Impl::get_mount_by_name(MountByType mount_by_type) const
    {
	string ret;

	switch (mount_by_type)
	{
	    case MountByType::UUID:
		if (!uuid.empty())
		    ret = "UUID=" + uuid;
		else
		    y2war("no uuid defined, using fallback");
		break;

	    case MountByType::LABEL:
		if (!label.empty())
		    ret = "LABEL=" + label;
		else
		    y2war("no label defined, using fallback");
		break;

	    case MountByType::ID:
	    case MountByType::PATH:
	    case MountByType::DEVICE:
		break;
	}

	if (ret.empty())
	{
	    ret = Encryption::Impl::get_mount_by_name(mount_by_type);
	}

	return ret;
    }


    namespace
    {

	/**
	 * Activation information about LUKS.
	 */
	struct LuksActivationInfo
	{
	    bool canceled = false;
	    string password;
	};


	/**
	 * Map with activation information about LUKS using the LUKS UUID as the key.
	 */
	map<string, LuksActivationInfo> luks_activation_infos;

    }


    void
    Luks::Impl::reset_activation_infos()
    {
	luks_activation_infos.clear();
    }


    bool
    Luks::Impl::activate_luks(const ActivateCallbacks* activate_callbacks, SystemInfo::Impl& system_info,
			      const string& name, const string& uuid, const string& label)
    {
	LuksInfo luks_info;
	luks_info.get_impl().device_name = name;
	luks_info.get_impl().size = system_info.getCmdBlockdev(name).get_size();
	luks_info.get_impl().uuid = uuid;
	luks_info.get_impl().label = label;

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab();
	const CrypttabEntry* crypttab_entry = etc_crypttab.find_by_any_block_device(system_info, uuid,
										    label, majorminor);

	string dm_table_name;

	if (crypttab_entry)
	{
	    dm_table_name = luks_info.get_impl().dm_table_name = crypttab_entry->get_crypt_device();
	    luks_info.get_impl().is_dm_table_name_generated = false;
	}
	else
	{
	    dm_table_name = luks_info.get_impl().dm_table_name = next_free_cr_auto_name(system_info);
	    luks_info.get_impl().is_dm_table_name_generated = true;
	}

	map<string, LuksActivationInfo>::const_iterator it = luks_activation_infos.find(uuid);

	int attempt = 1;

	while (true)
	{
	    string password;

	    if (it == luks_activation_infos.end())
	    {
		pair<bool, string> tmp;

		if (dynamic_cast<const ActivateCallbacksLuks*>(activate_callbacks))
		{
		    tmp = dynamic_cast<const ActivateCallbacksLuks*>(activate_callbacks)->luks(luks_info, attempt);
		}
		else
		{
		    tmp = activate_callbacks->luks(uuid, attempt);
		}

		if (!tmp.first)
		{
		    y2mil("user canceled activation of luks " << uuid);
		    luks_activation_infos[uuid].canceled = true;

		    return false;
		}
		else
		{
		    y2mil("user allowed activation of luks " << uuid);
		    password = tmp.second;
		}
	    }
	    else if (it->second.canceled)
	    {
		y2mil("activation of luks " << uuid << " skipped since previously canceled");
		return false;
	    }
	    else
	    {
		y2mil("activation of luks " << uuid << " with cached password");
		password = it->second.password;
	    }

	    // TRANSLATORS: progress message
	    message_callback(activate_callbacks, sformat(_("Activating LUKS %s"), uuid));

	    string cmd_line = CRYPTSETUP_BIN " --batch-mode open --type luks " + quote(name) + " " +
		quote(dm_table_name) + " --tries 1 --key-file -";

	    SystemCmd::Options cmd_options(cmd_line, SystemCmd::DoThrow);
	    cmd_options.stdin_text = password;
	    cmd_options.verify = [](int exit_code) { return exit_code == 0 || exit_code == 2; };

	    try
	    {
		SystemCmd cmd(cmd_options);

		// check for wrong password
		if (cmd.retcode() == 2)
		{
		    attempt++;
		    continue;
		}

		// save the password only if it is correct
		luks_activation_infos[uuid].password = password;

		return true;
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		// TRANSLATORS: error message
		error_callback(activate_callbacks, sformat(_("Activating LUKS %s failed"), uuid),
			       exception);

		return false;
	    }
	}
    }


    bool
    Luks::Impl::activate_lukses(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_lukses");

	try
	{
	    SystemInfo::Impl system_info;

	    bool ret = false;

	    for (const Blkid::value_type& key_value1 : system_info.getBlkid())
	    {
		if (!key_value1.second.is_luks)
		    continue;

		// Check whether the block device has holders (as reported in
		// /sys by the kernel). In that case it is either already
		// activated (the LUKS is already opened) or it is used by
		// something else, e.g. multipath. In any case it must be
		// skipped.

		if (has_kernel_holders(key_value1.first, system_info))
		    continue;

		y2mil("inactive luks name:" << key_value1.first << " uuid:" <<
		      key_value1.second.luks_uuid << " label:" << key_value1.second.luks_label);

		if (activate_luks(activate_callbacks, system_info, key_value1.first,
				  key_value1.second.luks_uuid, key_value1.second.luks_label))
		    ret = true;
	    }

	    if (ret)
		SystemCmd(UDEVADM_BIN_SETTLE);

	    return ret;
	}
	catch (const Exception& exception)
	{
	    ST_CAUGHT(exception);

	    if (typeid(exception) == typeid(Aborted))
		ST_RETHROW(exception);

	    // Ignore failure to detect whether LUKSes needs to be activated.

	    return false;
	}
    }


    bool
    Luks::Impl::deactivate_lukses()
    {
	y2mil("deactivate_lukses");

	bool ret = true;

	SystemInfo::Impl system_info;

	for (const CmdDmsetupInfo::value_type& value : system_info.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem != "CRYPT")
		continue;

	    if (!boost::starts_with(value.second.uuid, "CRYPT-LUKS"))
		continue;

	    string cmd_line = CRYPTSETUP_BIN " --batch-mode close " + quote(value.first);

	    SystemCmd cmd(cmd_line);

	    if (cmd.retcode() != 0)
		ret = false;
	}

	return ret;
    }


    void
    Luks::Impl::probe_lukses(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();
	const CmdDmsetupTable& cmd_dmsetup_table = system_info.getCmdDmsetupTable();
	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab();
	const Blkid& blkid = system_info.getBlkid();

	/*
	 * The main source for probing LUKSes is the blkid output. It contains
	 * all LUKSes with UUID, label and underlying block device. For some LUKSes
	 * this is already all available information.
	 *
	 * Only search for LUKSes on block devices without children. Otherwise
	 * e.g. LUKSes directly on multipath devices would be detected several
	 * times.
	 */

	for (BlkDevice* blk_device : BlkDevice::get_all(prober.get_system()))
	{
	    if (blk_device->has_children())
		continue;

	    if (!blk_device->get_impl().is_active())
		continue;

	    Blkid::const_iterator it1 = blkid.find_by_any_name(blk_device->get_name(), system_info);
	    if (it1 == blkid.end() || !it1->second.is_luks)
		continue;

	    string uuid = it1->second.luks_uuid;
	    string label = it1->second.luks_label;

	    dev_t majorminor = system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor();

	    CmdDmsetupTable::const_iterator it2 = cmd_dmsetup_table.find_using(majorminor);

	    const CrypttabEntry* crypttab_entry = etc_crypttab.find_by_any_block_device(system_info, uuid,
											label, majorminor);

	    /*
	     * The DM table name is 1. taken from the active table, 2. taken
	     * from crypttab and 3. auto-generated.
	     */
	    string dm_table_name;
	    if (it2 != cmd_dmsetup_table.end())
	    {
		dm_table_name = it2->first;

		/*
		 * If integrity is set the name will ends with _dif, and later we will
		 * find the target with the correct name.
		 */
		if (boost::ends_with(dm_table_name, "_dif"))
		{
		    bool integrity = false;
		    for (const CmdDmsetupTable::Table& table : it2->second)
		    {
			if (table.target == "integrity")
			    integrity = true;
		    }

		    if (integrity)
		    {
			dm_table_name = dm_table_name.substr(0, dm_table_name.length() - 4);
			const CmdDmsetupInfo& cmd_dmsetup_info = system_info.getCmdDmsetupInfo();
			if (!cmd_dmsetup_info.exists(dm_table_name))
			    ST_THROW(Exception("integrity device found, missing the associated 'crypt' target"));
		    }
		}
	    }
	    else if (crypttab_entry)
		dm_table_name = crypttab_entry->get_crypt_device();
	    else
		dm_table_name = next_free_cr_auto_name(system_info);

	    Luks* luks = Luks::create(prober.get_system(), dm_table_name);
	    luks->get_impl().uuid = uuid;
	    luks->get_impl().label = label;
	    luks->get_impl().set_active(it2 != cmd_dmsetup_table.end());
	    luks->set_in_etc_crypttab(crypttab_entry);

	    /*
	     * Copy the password if it is known from activation.
	     */
	    const map<string, LuksActivationInfo>::const_iterator it = luks_activation_infos.find(uuid);
	    if (it != luks_activation_infos.end() && !it->second.canceled)
		luks->set_password(it->second.password);

	    const CmdCryptsetupLuksDump& cmd_cryptsetup_luks_dump = system_info.getCmdCryptsetupLuksDump(blk_device->get_name());
	    luks->get_impl().Encryption::Impl::set_type(cmd_cryptsetup_luks_dump.get_encryption_type());
	    luks->get_impl().set_cipher(cmd_cryptsetup_luks_dump.get_cipher());
	    luks->get_impl().set_key_size(cmd_cryptsetup_luks_dump.get_key_size());
	    luks->get_impl().set_pbkdf(cmd_cryptsetup_luks_dump.get_pbkdf());
	    luks->get_impl().set_integrity(cmd_cryptsetup_luks_dump.get_integrity());

	    if (crypttab_entry)
	    {
		luks->set_mount_by(crypttab_entry->get_mount_by());
		luks->get_impl().set_crypt_options(crypttab_entry->get_crypt_opts());
		luks->get_impl().set_crypttab_blk_device_name(crypttab_entry->get_block_device());

		string password = crypttab_entry->get_password();
		if (!password.empty())
		    luks->set_key_file(password);
	    }

	    prober.add_holder(blk_device->get_name(), luks, [](Devicegraph* system, Device* a, Device* b) {
		User::create(system, a, b);
	    });

	    luks->get_impl().probe_pass_1a(prober);
	}
    }


    void
    Luks::Impl::probe_uuid()
    {
	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid(blk_device->get_name());
	Blkid::const_iterator it = blkid.get_sole_entry();
	if (it != blkid.end())
	    uuid = it->second.luks_uuid;
    }


    void
    Luks::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region_and_topology();
    }


    unsigned long long
    Luks::Impl::metadata_size() const
    {
	switch (get_type())
	{
	    case EncryptionType::LUKS1:
		return v1_metadata_size;

	    case EncryptionType::LUKS2:
		return v2_metadata_size;

	    default:
		ST_THROW(Exception("invalid encryption type"));
	}
    }


    void
    Luks::Impl::calculate_region_and_topology()
    {
	// We return early for the same reason that we did in
	// Md::Impl::calculate_region_and_topology()
	if (exists_in_system())
	    return;

	const BlkDevice* blk_device = get_blk_device();

	unsigned long long size = blk_device->get_size();

	// size of luks metadata is explained at
	// https://gitlab.com/cryptsetup/cryptsetup/wikis/FrequentlyAskedQuestions

	if (size > metadata_size())
	    size -= metadata_size();
	else
	    size = 0 * B;

	// If we use integrity, calculate a pesimistic size
	if (!get_integrity().empty())
	    size *= 0.85;

	set_size(size);

	// alignment_offset is 0 even is the underlying blk device
	// (e.g. a partition) has alignment_offset !=
	// 0. optimal_io_size is the same as for the underlying blk
	// device.

	set_topology(Topology(0, blk_device->get_topology().get_optimal_io_size()));
    }


    bool
    Luks::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Encryption::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid && label == rhs.label && format_options == rhs.format_options;
    }


    void
    Luks::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Encryption::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
	storage::log_diff(log, "label", label, rhs.label);

	storage::log_diff(log, "format-options", format_options, rhs.format_options);
    }


    void
    Luks::Impl::print(std::ostream& out) const
    {
	Encryption::Impl::print(out);

	out << " uuid:" << uuid;

	if (!label.empty())
	    out << " label:" << label;

	if (!format_options.empty())
	    out << " format-options:" << format_options;
    }


    bool
    Luks::Impl::do_resize_needs_password() const
    {
	// Resizing an LUKS2 device might require the password
	// (depending on use of kernel keyring).

	return get_type() == EncryptionType::LUKS2;
    }


    ResizeInfo
    Luks::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	// TODO Strictly speaking "empty" does not mean "unknown".
	if (do_resize_needs_password() && get_password().empty())
	    return ResizeInfo(false, RB_PASSWORD_REQUIRED);

	ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info(get_non_impl());

	resize_info.shift(metadata_size());

	return resize_info;
    }


    uf_t
    Luks::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_LUKS | Encryption::Impl::used_features(used_features_dependency_type);
    }


    void
    Luks::Impl::do_create()
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode luksFormat " + quote(blk_device->get_name());

	switch (get_type())
	{
	    case EncryptionType::LUKS1:
		cmd_line += " --type luks1";
		break;

	    case EncryptionType::LUKS2:
		cmd_line += " --type luks2";
		if (!label.empty())
		    cmd_line += " --label " + quote(label);
		break;

	    default:
		ST_THROW(Exception("invalid encryption type"));
	}

	cmd_line += " --tries 1";

	if (!uuid.empty())
	    cmd_line += " --uuid " + quote(uuid);

	if (!get_cipher().empty())
	    cmd_line += " --cipher " + quote(get_cipher());

	if (get_key_size() != 0)
	    cmd_line += " --key-size " + to_string(get_key_size() * 8);

	if (!get_pbkdf().empty())
	    cmd_line += " --pbkdf " + quote(get_pbkdf());

	// TODO(check the LUKS version and the cipher)
	if (!get_integrity().empty())
	    cmd_line += " --integrity " + quote(get_integrity());

	if (!get_format_options().empty())
	    cmd_line += " " + get_format_options();

	add_key_file_option_and_execute(cmd_line);

	if (uuid.empty())
	{
	    probe_uuid();
	}
    }


    void
    Luks::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode erase " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	// cryptsetup erase does not remove the signature, thus also use
	// generic wipefs.

	blk_device->get_impl().wipe_device();
    }


    void
    Luks::Impl::do_activate() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode open --type luks " + quote(blk_device->get_name()) +
	    " " + quote(get_dm_table_name()) + " --tries 1 " + get_open_options();

	add_key_file_option_and_execute(cmd_line);
    }


    void
    Luks::Impl::do_deactivate() const
    {
	string cmd_line = CRYPTSETUP_BIN " --batch-mode close " + quote(get_dm_table_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}

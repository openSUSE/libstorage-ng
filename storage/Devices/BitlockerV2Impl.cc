/*
 * Copyright (c) 2022 SUSE LLC
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
#include "storage/Devices/BitlockerV2Impl.h"
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


    const char* DeviceTraits<BitlockerV2>::classname = "BitlockerV2";


    BitlockerV2::Impl::Impl(const string& dm_table_name)
	: Encryption::Impl(dm_table_name)
    {
	set_type(EncryptionType::BITLOCKER);

	set_crypt_options(vector<string>( { "bitlk" } ));
    }


    BitlockerV2::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node)
    {
	getChildValue(node, "uuid", uuid);
    }


    string
    BitlockerV2::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("BitLocker").translated;
    }


    void
    BitlockerV2::Impl::save(xmlNode* node) const
    {
	Encryption::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    BitlockerV2::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Encryption::Impl::check(check_callbacks);

	if (get_type() != EncryptionType::BITLOCKER)
	    ST_THROW(Exception("invalid encryption type for BitlockerV2"));
    }


    string
    BitlockerV2::Impl::get_mount_by_name(MountByType mount_by_type) const
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
	 * Activation information about BitLocker.
	 */
	struct BitlockerActivationInfo
	{
	    bool canceled = false;
	    string password;
	};


	/**
	 * Map with activation information about BitLocker using the BitLocker UUID as the key.
	 */
	map<string, BitlockerActivationInfo> bitlocker_activation_infos;

    }


    void
    BitlockerV2::Impl::reset_activation_infos()
    {
	bitlocker_activation_infos.clear();
    }


    bool
    BitlockerV2::Impl::activate_bitlocker(const ActivateCallbacksV3* activate_callbacks, SystemInfo::Impl& system_info,
					  const string& name, const string& uuid)
    {
	BitlockerInfo bitlocker_info;
	bitlocker_info.get_impl().device_name = name;
	bitlocker_info.get_impl().size = system_info.getCmdBlockdev(name).get_size();
	bitlocker_info.get_impl().uuid = uuid;

	dev_t majorminor = system_info.getCmdUdevadmInfo(name).get_majorminor();

	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab();
	const CrypttabEntry* crypttab_entry = etc_crypttab.find_by_any_block_device(system_info, uuid,
										    "", majorminor);

	string dm_table_name;

	if (crypttab_entry)
	{
	    dm_table_name = bitlocker_info.get_impl().dm_table_name = crypttab_entry->get_crypt_device();
	    bitlocker_info.get_impl().is_dm_table_name_generated = false;
	}
	else
	{
	    dm_table_name = bitlocker_info.get_impl().dm_table_name = next_free_cr_auto_name(system_info);
	    bitlocker_info.get_impl().is_dm_table_name_generated = true;
	}

	map<string, BitlockerActivationInfo>::const_iterator it = bitlocker_activation_infos.find(uuid);

	int attempt = 1;

	while (true)
	{
	    string password;

	    if (it == bitlocker_activation_infos.end())
	    {
		pair<bool, string> tmp = activate_callbacks->bitlocker(bitlocker_info, attempt);

		if (!tmp.first)
		{
		    y2mil("user canceled activation of bitlocker " << uuid);
		    bitlocker_activation_infos[uuid].canceled = true;

		    return false;
		}
		else
		{
		    y2mil("user allowed activation of bitlocker " << uuid);
		    password = tmp.second;
		}
	    }
	    else if (it->second.canceled)
	    {
		y2mil("activation of bitlocker " << uuid << " skipped since previously canceled");
		return false;
	    }
	    else
	    {
		y2mil("activation of bitlocker " << uuid << " with cached password");
		password = it->second.password;
	    }

	    // TRANSLATORS: progress message
	    message_callback(activate_callbacks, sformat(_("Activating BitLocker %s"), uuid));


	    string cmd_line = CRYPTSETUP_BIN " --batch-mode open --type bitlk " + quote(name) + " " +
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
		bitlocker_activation_infos[uuid].password = password;

		return true;
	    }
	    catch (const Exception& exception)
	    {
		ST_CAUGHT(exception);

		// TRANSLATORS: error message
		error_callback(activate_callbacks, sformat(_("Activating BitLocker %s failed"), uuid),
			       exception);

		return false;
	    }
	}
    }


    bool
    BitlockerV2::Impl::activate_bitlockers(const ActivateCallbacksV3* activate_callbacks)
    {
	y2mil("activate_bitlockers");

	try
	{
	    SystemInfo::Impl system_info;

	    bool ret = false;

	    for (const Blkid::value_type& key_value1 : system_info.getBlkid())
	    {
		if (!key_value1.second.is_bitlocker)
		    continue;

		// Check whether the block device has holders (as reported in
		// /sys by the kernel). In that case it is either already
		// activated (the BitLocker is already opened) or it is used by
		// something else, e.g. multipath. In any case it must be
		// skipped.

		if (has_kernel_holders(key_value1.first, system_info))
		    continue;

		const CmdCryptsetupBitlkDump& cmd_cryptsetup_bitlk_dump =
		    system_info.getCmdCryptsetupBitlkDump(key_value1.first);

		string uuid = cmd_cryptsetup_bitlk_dump.get_uuid();

		y2mil("inactive bitlocker name:" << key_value1.first << " uuid:" << uuid);

		if (activate_bitlocker(activate_callbacks, system_info, key_value1.first, uuid))
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

	    // Ignore failure to detect whether BitLockers needs to be activated.

	    return false;
	}
    }


    bool
    BitlockerV2::Impl::deactivate_bitlockers()
    {
	y2mil("deactivate_bitlockers");

	bool ret = true;

	SystemInfo::Impl system_info;

	for (const CmdDmsetupInfo::value_type& value : system_info.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem != "CRYPT")
		continue;

	    if (!boost::starts_with(value.second.uuid, "CRYPT-BITLK"))
		continue;

	    string cmd_line = CRYPTSETUP_BIN " --batch-mode close " + quote(value.first);

	    SystemCmd cmd(cmd_line);

	    if (cmd.retcode() != 0)
		ret = false;
	}

	return ret;
    }


    void
    BitlockerV2::Impl::probe_bitlockers(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();
	const CmdDmsetupTable& cmd_dmsetup_table = system_info.getCmdDmsetupTable();
	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab();
	const Blkid& blkid = system_info.getBlkid();

	/*
	 * The main source for probing BitLockers is the blkid output. It contains all
	 * BitLockers with the underlying block device. The UUID is queried using
	 * 'cryptsetup bitlkDump'.
	 *
	 * Only search for BitLockers on block devices without children. Otherwise
	 * e.g. BitLockers directly on multipath devices would be detected several times.
	 */

	for (BlkDevice* blk_device : BlkDevice::get_all(prober.get_system()))
	{
	    if (blk_device->has_children())
		continue;

	    if (!blk_device->get_impl().is_active())
		continue;

	    Blkid::const_iterator it1 = blkid.find_by_any_name(blk_device->get_name(), system_info);
	    if (it1 == blkid.end() || !it1->second.is_bitlocker)
		continue;

	    dev_t majorminor = system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor();

	    CmdDmsetupTable::const_iterator it2 = cmd_dmsetup_table.find_using(majorminor);

	    const CmdCryptsetupBitlkDump& cmd_cryptsetup_bitlk_dump =
		system_info.getCmdCryptsetupBitlkDump(blk_device->get_name());

	    string uuid = cmd_cryptsetup_bitlk_dump.get_uuid();

	    const CrypttabEntry* crypttab_entry = etc_crypttab.find_by_any_block_device(system_info, uuid,
											"", majorminor);

	    /*
	     * The DM table name is 1. taken from the active table, 2. taken
	     * from crypttab and 3. auto-generated.
	     */
	    string dm_table_name;
	    if (it2 != cmd_dmsetup_table.end())
		dm_table_name = it2->first;
	    else if (crypttab_entry)
		dm_table_name = crypttab_entry->get_crypt_device();
	    else
		dm_table_name = next_free_cr_auto_name(system_info);

	    BitlockerV2* bitlocker = BitlockerV2::create(prober.get_system(), dm_table_name);
	    bitlocker->get_impl().uuid = uuid;
	    bitlocker->get_impl().set_active(it2 != cmd_dmsetup_table.end());
	    bitlocker->set_in_etc_crypttab(crypttab_entry);

	    /*
	     * Copy the password if it is known from activation.
	     */
	    const map<string, BitlockerActivationInfo>::const_iterator it = bitlocker_activation_infos.find(uuid);
	    if (it != bitlocker_activation_infos.end() && !it->second.canceled)
		bitlocker->set_password(it->second.password);

	    bitlocker->get_impl().set_cipher(cmd_cryptsetup_bitlk_dump.get_cipher());
	    bitlocker->get_impl().set_key_size(cmd_cryptsetup_bitlk_dump.get_key_size());

	    if (crypttab_entry)
	    {
		bitlocker->set_mount_by(crypttab_entry->get_mount_by());
		bitlocker->get_impl().set_crypt_options(crypttab_entry->get_crypt_opts());
		bitlocker->get_impl().set_crypttab_blk_device_name(crypttab_entry->get_block_device());

		string password = crypttab_entry->get_password();
		if (!password.empty())
		    bitlocker->set_key_file(password);
	    }

	    prober.add_holder(blk_device->get_name(), bitlocker, [](Devicegraph* system, Device* a, Device* b) {
		User::create(system, a, b);
	    });

	    bitlocker->get_impl().probe_pass_1a(prober);
	}
    }


    bool
    BitlockerV2::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Encryption::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    BitlockerV2::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Encryption::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    BitlockerV2::Impl::print(std::ostream& out) const
    {
	Encryption::Impl::print(out);

	out << " uuid:" << uuid;
    }


    ResizeInfo
    BitlockerV2::Impl::detect_resize_info(const BlkDevice* blk_device) const
    {
	return ResizeInfo(false, RB_RESIZE_NOT_SUPPORTED_BY_DEVICE);
    }


    uf_t
    BitlockerV2::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_BITLOCKER | Encryption::Impl::used_features(used_features_dependency_type);
    }


    void
    BitlockerV2::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode erase " + quote(blk_device->get_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);

	// cryptsetup erase does not remove the signature, thus also use
	// generic wipefs.

	blk_device->get_impl().wipe_device();
    }


    void
    BitlockerV2::Impl::do_activate() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode open --type bitlk " + quote(blk_device->get_name()) +
	    " " + quote(get_dm_table_name()) + " --tries 1 " + get_open_options();

	add_key_file_option_and_execute(cmd_line);
    }


    void
    BitlockerV2::Impl::do_deactivate() const
    {
	string cmd_line = CRYPTSETUP_BIN " --batch-mode close " + quote(get_dm_table_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}

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


#include <iostream>

#include "storage/Utils/XmlFile.h"
#include "storage/Utils/SystemCmd.h"
#include "storage/Utils/HumanString.h"
#include "storage/Devices/LuksImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"
#include "storage/FreeInfo.h"
#include "storage/StorageImpl.h"
#include "storage/SystemInfo/SystemInfo.h"
#include "storage/UsedFeatures.h"
#include "storage/EtcCrypttab.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<Luks>::classname = "Luks";


    Luks::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node), uuid()
    {
	getChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::save(xmlNode* node) const
    {
	Encryption::Impl::save(node);

	setChildValue(node, "uuid", uuid);
    }


    void
    Luks::Impl::check() const
    {
	BlkDevice::Impl::check();

	if (!has_single_parent_of_type<const BlkDevice>())
	    ST_THROW(Exception("Luks has no BlkDevice parent"));

	if (get_size() + metadata_size > get_blk_device()->get_size())
	    ST_THROW(Exception("Luks bigger than parent BlkDevice"));
    }


    string
    Luks::Impl::next_free_cr_auto_name(SystemInfo& systeminfo)
    {
	static int nr = 1;

	while (true)
	{
	    string name = "cr-auto-" + to_string(nr++);

	    if (!systeminfo.getCmdDmsetupInfo().exists(name))
		return name;
	}
    }


    bool
    Luks::Impl::activate_luks(const ActivateCallbacks* activate_callbacks, SystemInfo& systeminfo,
			      const string& name, const string& uuid)
    {
	int attempt = 1;
	string dm_name;

	while (true)
	{
	    pair<bool, string> tmp = activate_callbacks->luks(uuid, attempt);
	    if (!tmp.first)
		return false;

	    if (attempt == 1)
		dm_name = next_free_cr_auto_name(systeminfo);

	    string cmd_line = CRYPTSETUPBIN " --batch-mode luksOpen " + quote(name) + " " +
		quote(dm_name) + " --key-file -";
	    cout << cmd_line << endl;

	    SystemCmd cmd;
	    cmd.setStdinText(tmp.second);
	    cmd.execute(cmd_line);

	    // check for wrong password
	    if (cmd.retcode() == 2)
	    {
		attempt++;
		continue;
	    }

	    if (cmd.retcode() != 0)
		ST_THROW(Exception("activate Luks failed"));

	    return true;
	}
    }


    bool
    Luks::Impl::activate_lukses(const ActivateCallbacks* activate_callbacks)
    {
	y2mil("activate_lukses");

	SystemInfo systeminfo;

	bool ret = false;

	for (const Blkid::value_type& key_value1 : systeminfo.getBlkid())
	{
	    if (!key_value1.second.is_luks)
		continue;

	    // major and minor of the device holding the luks
	    dev_t majorminor = systeminfo.getCmdUdevadmInfo(key_value1.first).get_majorminor();

	    const CmdDmsetupTable& dmsetup_table = systeminfo.getCmdDmsetupTable();
	    CmdDmsetupTable::const_iterator it = dmsetup_table.find_using(majorminor);
	    if (it != dmsetup_table.end())
		continue;

	    y2mil("inactive luks name:" << key_value1.first << " uuid:" <<
		  key_value1.second.luks_uuid);

	    // TODO During a second loop of Storage::Impl::activate() the
	    // library should not bother the user with popups to lukses where
	    // an activation was canceled by the user. Maybe the library
	    // should also remember the passwords (map<uuid, password>) in
	    // case the activation is run again, e.g. after deactivation and
	    // reprobe.

	    if (activate_luks(activate_callbacks, systeminfo, key_value1.first,
			      key_value1.second.luks_uuid))
		ret = true;
	}

	if (ret)
	    SystemCmd(UDEVADMBIN_SETTLE);

	return ret;
    }


    void
    Luks::Impl::probe_lukses(Devicegraph* probed, SystemInfo& systeminfo)
    {
	for (const CmdDmsetupInfo::value_type& value : systeminfo.getCmdDmsetupInfo())
	{
	    if (value.second.subsystem != "CRYPT")
		continue;

	    const CmdCryptsetup& cmd_cryptsetup = systeminfo.getCmdCryptsetup(value.first);
	    if (cmd_cryptsetup.encryption_type != EncryptionType::LUKS)
		continue;

	    Luks* luks = Luks::create(probed, value.first);
	    luks->get_impl().probe_pass_1(probed, systeminfo);
	}
    }


    void
    Luks::Impl::probe_pass_1(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_1(probed, systeminfo);

	const File size_file = systeminfo.getFile(SYSFSDIR + get_sysfs_path() + "/size");

	set_region(Region(0, size_file.get_unsigned_long_long(), 512));

	const EtcCrypttab& etc_crypttab = systeminfo.getEtcCrypttab();
	set_in_etc_crypttab(etc_crypttab.has_crypt_device(get_dm_table_name()));
    }


    void
    Luks::Impl::probe_pass_2(Devicegraph* probed, SystemInfo& systeminfo)
    {
	Encryption::Impl::probe_pass_2(probed, systeminfo);

	const BlkDevice* blk_device = get_blk_device();

	const Blkid& blkid = systeminfo.getBlkid();
	Blkid::Entry entry2;
	if (!blkid.find_by_name(blk_device->get_name(), entry2, systeminfo))
	    ST_THROW(Exception("failed to probe luks uuid"));

	uuid = entry2.luks_uuid;
    }


    void
    Luks::Impl::probe_uuid()
    {
	const BlkDevice* blk_device = get_blk_device();

	Blkid blkid(blk_device->get_name());
	Blkid::Entry entry;
	if (blkid.get_sole_entry(entry))
	    uuid = entry.luks_uuid;
    }


    void
    Luks::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region();
    }


    void
    Luks::Impl::calculate_region()
    {
	const BlkDevice* blk_device = get_blk_device();

	unsigned long long size = blk_device->get_size();

	// size of luks metadata is explained at
	// https://gitlab.com/cryptsetup/cryptsetup/wikis/FrequentlyAskedQuestions

	if (size > metadata_size)
	    size -= metadata_size;
	else
	    size = 0 * B;

	set_size(size);
    }


    bool
    Luks::Impl::equal(const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	if (!Encryption::Impl::equal(rhs))
	    return false;

	return uuid == rhs.uuid;
    }


    void
    Luks::Impl::log_diff(std::ostream& log, const Device::Impl& rhs_base) const
    {
	const Impl& rhs = dynamic_cast<const Impl&>(rhs_base);

	Encryption::Impl::log_diff(log, rhs);

	storage::log_diff(log, "uuid", uuid, rhs.uuid);
    }


    void
    Luks::Impl::print(std::ostream& out) const
    {
	Encryption::Impl::print(out);

	out << " uuid:" << uuid;
    }


    ResizeInfo
    Luks::Impl::detect_resize_info() const
    {
	ResizeInfo resize_info = BlkDevice::Impl::detect_resize_info();

	// TODO handle metadata size

	return resize_info;
    }


    uint64_t
    Luks::Impl::used_features() const
    {
	return UF_LUKS | Encryption::Impl::used_features();
    }


    void
    Luks::Impl::do_create()
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode luksFormat " + quote(get_blk_device()->get_name()) +
	    " --key-file -";
	cout << cmd_line << endl;

	SystemCmd cmd;
	cmd.setStdinText(get_password());
	cmd.execute(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("create Luks failed"));

	probe_uuid();
    }


    void
    Luks::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUPBIN " --batch-mode erase " + quote(blk_device->get_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("delete Luks failed"));

	// cryptsetup erase does not remove the signature, thus also use
	// generic wipefs.

	blk_device->get_impl().wipe_device();
    }


    void
    Luks::Impl::do_resize(ResizeMode resize_mode, const Device* rhs) const
    {
	const Luks* luks_rhs = to_luks(rhs);

	string cmd_line = CRYPTSETUPBIN " resize " + quote(get_dm_table_name());

	if (resize_mode == ResizeMode::SHRINK)
	    cmd_line += " --size " + to_string(luks_rhs->get_impl().get_size() / (512 * B));

	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("resize Luks failed"));
    }


    void
    Luks::Impl::do_activate() const
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode luksOpen " + quote(get_blk_device()->get_name()) + " " +
	    quote(get_dm_table_name()) + " --key-file -";
	cout << cmd_line << endl;

	SystemCmd cmd;
	cmd.setStdinText(get_password());
	cmd.execute(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("activate Luks failed"));
    }


    void
    Luks::Impl::do_deactivate() const
    {
	string cmd_line = CRYPTSETUPBIN " --batch-mode close " + quote(get_dm_table_name());
	cout << cmd_line << endl;

	SystemCmd cmd(cmd_line);
	if (cmd.retcode() != 0)
	    ST_THROW(Exception("deactivate Luks failed"));
    }


    void
    Luks::Impl::do_add_to_etc_crypttab(CommitData& commit_data) const
    {
	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

	// TODO, error handling and mount-by

        CrypttabEntry * entry = new CrypttabEntry();
        entry->set_crypt_device(get_dm_table_name());
        entry->set_block_device( get_blk_device()->get_name() );

        etc_crypttab.add(entry);
        etc_crypttab.log();
        etc_crypttab.write();
    }


    void
    Luks::Impl::do_rename_in_etc_crypttab(CommitData& commit_data, const Device* lhs) const
    {
	const Luks* luks_lhs = to_luks(lhs);

	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

	// TODO, error handling and mount-by

        string old_block_device = luks_lhs->get_blk_device()->get_name();
        CrypttabEntry* entry = etc_crypttab.find_block_device(old_block_device);

        if (entry)
        {
            entry->set_block_device(get_blk_device()->get_name());
            etc_crypttab.log();
            etc_crypttab.write();
        }
    }


    void
    Luks::Impl::do_remove_from_etc_crypttab(CommitData& commit_data) const
    {
	EtcCrypttab& etc_crypttab = commit_data.get_etc_crypttab();

        CrypttabEntry* entry = etc_crypttab.find_block_device(get_blk_device()->get_name());

        if (entry)
        {
            etc_crypttab.remove(entry);
            etc_crypttab.log();
            etc_crypttab.write();
        }
    }

}

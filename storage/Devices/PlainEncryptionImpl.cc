/*
 * Copyright (c) [2019-2022] SUSE LLC
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
#include "storage/Utils/SystemCmd.h"
#include "storage/Devices/PlainEncryptionImpl.h"
#include "storage/Holders/User.h"
#include "storage/Devicegraph.h"
#include "storage/SystemInfo/SystemInfoImpl.h"
#include "storage/UsedFeatures.h"
#include "storage/EtcCrypttab.h"
#include "storage/Prober.h"
#include "storage/Utils/Format.h"
#include "storage/Storage.h"


namespace storage
{

    using namespace std;


    const char* DeviceTraits<PlainEncryption>::classname = "PlainEncryption";


    PlainEncryption::Impl::Impl(const xmlNode* node)
	: Encryption::Impl(node)
    {
    }


    string
    PlainEncryption::Impl::get_pretty_classname() const
    {
	// TRANSLATORS: name of object
	return _("Plain Encryption").translated;
    }


    void
    PlainEncryption::Impl::check(const CheckCallbacks* check_callbacks) const
    {
	Encryption::Impl::check(check_callbacks);

	if (get_type() != EncryptionType::PLAIN)
	    ST_THROW(Exception("invalid encryption type for PlainEncryption"));
    }


    void
    PlainEncryption::Impl::probe_plain_encryptions(Prober& prober)
    {
	SystemInfo::Impl& system_info = prober.get_system_info();
	const Storage& storage = prober.get_storage();
	const CmdDmsetupTable& cmd_dmsetup_table = system_info.getCmdDmsetupTable();
	const EtcCrypttab& etc_crypttab = system_info.getEtcCrypttab(storage.prepend_rootprefix(ETC_CRYPTTAB));
	const Blkid& blkid = system_info.getBlkid();

	/*
	 * The code for probing plain encryptions is similar to the code probing LUKSes.
	 * The main difference is that plain encryptions are not reported by blkid.
	 */

	for (BlkDevice* blk_device : BlkDevice::get_all(prober.get_system()))
	{
	    if (blk_device->has_children())
		continue;

	    if (!blk_device->get_impl().is_active())
		continue;

	    // If the blk device is included in the output of blkid it
	    // cannot be a plain encryption.
	    Blkid::const_iterator it1 = blkid.find_by_any_name(blk_device->get_name(), system_info);
	    if (it1 != blkid.end())
		continue;

	    dev_t majorminor = system_info.getCmdUdevadmInfo(blk_device->get_name()).get_majorminor();

	    CmdDmsetupTable::const_iterator it2 = cmd_dmsetup_table.find_using(majorminor);

	    if (it2 != cmd_dmsetup_table.end() && it2->second[0].target != "crypt")
		it2 = cmd_dmsetup_table.end();

	    const CrypttabEntry* crypttab_entry = etc_crypttab.find_by_any_block_device(system_info,
											"", "", majorminor);

	    /*
	     * The DM table name is 1. taken from the active table and 2. taken
	     * from crypttab
	     */
	    string dm_table_name;
	    if (it2 != cmd_dmsetup_table.end())
		dm_table_name = it2->first;
	    else if (crypttab_entry)
		dm_table_name = crypttab_entry->get_crypt_device();
	    else
		continue;

	    PlainEncryption* plain_encryption = PlainEncryption::create(prober.get_system(), dm_table_name);
	    plain_encryption->get_impl().set_active(it2 != cmd_dmsetup_table.end());
	    plain_encryption->set_in_etc_crypttab(crypttab_entry);

	    if (crypttab_entry)
	    {
		plain_encryption->set_mount_by(crypttab_entry->get_mount_by());
		plain_encryption->get_impl().set_crypt_options(crypttab_entry->get_crypt_opts());
		plain_encryption->get_impl().set_crypttab_blk_device_name(crypttab_entry->get_block_device());

		string password = crypttab_entry->get_password();
		if (!password.empty())
		    plain_encryption->set_key_file(password);
	    }

	    prober.add_holder(blk_device->get_name(), plain_encryption, [](Devicegraph* system, Device* a, Device* b) {
		User::create(system, a, b);
	    });

	    plain_encryption->get_impl().probe_pass_1a(prober);
	}
    }


    void
    PlainEncryption::Impl::parent_has_new_region(const Device* parent)
    {
	calculate_region_and_topology();
    }


    void
    PlainEncryption::Impl::calculate_region_and_topology()
    {
	const BlkDevice* blk_device = get_blk_device();

	set_size(blk_device->get_size());

	set_topology(blk_device->get_topology());
    }


    uf_t
    PlainEncryption::Impl::used_features(UsedFeaturesDependencyType used_features_dependency_type) const
    {
	return UF_PLAIN_ENCRYPTION | Encryption::Impl::used_features(used_features_dependency_type);
    }


    void
    PlainEncryption::Impl::do_create()
    {
	// Write random data to the beginning of the underlying blk
	// device. Otherwise the distribution of data on the blk
	// device can already reveal information about the
	// content. Taken from old libstorage.

	do_delete();
    }


    void
    PlainEncryption::Impl::do_delete() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = DD_BIN " if=/dev/urandom of=" + quote(blk_device->get_name()) +
	    " bs=1KiB count=256 conv=nocreat";

	wait_for_devices({ blk_device });

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }


    void
    PlainEncryption::Impl::do_activate() const
    {
	const BlkDevice* blk_device = get_blk_device();

	string cmd_line = CRYPTSETUP_BIN " --batch-mode plainOpen " + quote(blk_device->get_name()) + " "
	    + quote(get_dm_table_name()) + " --tries 1 " + get_open_options();

	add_key_file_option_and_execute(cmd_line);
    }


    void
    PlainEncryption::Impl::do_deactivate() const
    {
	string cmd_line = CRYPTSETUP_BIN " --batch-mode close " + quote(get_dm_table_name());

	SystemCmd cmd(cmd_line, SystemCmd::DoThrow);
    }

}

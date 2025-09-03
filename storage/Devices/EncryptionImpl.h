/*
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


#ifndef STORAGE_ENCRYPTION_IMPL_H
#define STORAGE_ENCRYPTION_IMPL_H


#include "storage/Utils/Enum.h"
#include "storage/Utils/StorageDefines.h"
#include "storage/Devices/Encryption.h"
#include "storage/Devices/BlkDeviceImpl.h"
#include "storage/EtcCrypttab.h"
#include "storage/Actions/RenameInImpl.h"


namespace storage
{

    using namespace std;


    template <> struct DeviceTraits<Encryption> { static const char* classname; };

    template <> struct EnumTraits<EncryptionType> { static const vector<string> names; };


    class Encryption::Impl : public BlkDevice::Impl
    {
    public:

	Impl(const string& dm_table_name);
	Impl(const xmlNode* node);

	virtual const char* get_classname() const override { return "Encryption"; }

	virtual string get_pretty_classname() const override;

	virtual string get_displayname() const override { return get_dm_table_name(); }

	virtual void probe_pass_1a(Prober& prober) override;

	/**
	 * Redefined from BlkDeviceImpl to ensure the device name is in sync
	 * with the DeviceMapper name, since the kernel device names for
	 * Encryption devices are inferred from their DeviceMapper names.
	 */
	virtual void set_dm_table_name(const string& dm_table_name) override;

	EncryptionType get_type() const { return type; }
	virtual void set_type(EncryptionType type) { Impl::type = type; }

	const string& get_password() const { return password; }
	void set_password(const string& password) { Impl::password = password; }

	const string& get_key_file() const { return key_file; }
	void set_key_file(const string& key_file) { Impl::key_file = key_file; }

	bool is_use_key_file_in_commit() const { return use_key_file_in_commit; }
	void set_use_key_file_in_commit(bool use_key_file_in_commit) { Impl::use_key_file_in_commit = use_key_file_in_commit; }

	const string& get_open_options() const { return open_options; }
	void set_open_options(const string& open_options) { Impl::open_options = open_options; }

	const string& get_cipher() const { return cipher; }
	void set_cipher(const string& cipher) { Impl::cipher = cipher; }

	unsigned int get_key_size() const { return key_size; }
	void set_key_size(unsigned int key_size) { Impl::key_size = key_size; }

	const string& get_pbkdf() const { return pbkdf; }
	void set_pbkdf(const string& pbkdf) { Impl::pbkdf = pbkdf; }

	const string& get_integrity() const { return integrity; }
	virtual void set_integrity(const string& integrity) { Impl::integrity = integrity; }

	MountByType get_mount_by() const { return mount_by; }
	void set_mount_by(MountByType mount_by) { Impl::mount_by = mount_by; }

	void set_default_mount_by();

	/**
	 * Value for the second field in /etc/crypttab (which we simply call spec in
	 * analogy to /etc/fstab).
	 */
	virtual string get_crypttab_spec(MountByType mount_by_type) const;

	const CryptOpts& get_crypt_options() const { return crypt_options; }
	void set_crypt_options(const CryptOpts& crypt_options);
	void set_crypt_options(const vector<string>& crypt_options);

	bool is_in_etc_crypttab() const { return in_etc_crypttab; }
	void set_in_etc_crypttab(bool in_etc_crypttab) { Impl::in_etc_crypttab = in_etc_crypttab; }

	/**
	 * Get the block device name that was used in /etc/crypttab. This is
	 * empty if this encryption was not in /etc/crypttab during probing.
	 */
        const string& get_crypttab_blk_device_name() const { return crypttab_blk_device_name; }
        void set_crypttab_blk_device_name(const string& name) { Impl::crypttab_blk_device_name = name; }

	BlkDevice* get_blk_device();
	const BlkDevice* get_blk_device() const;

	/**
	 * Does do_resize need the password?
	 */
	virtual bool do_resize_needs_password() const { return false; }

	virtual unique_ptr<Device::Impl> clone() const override { return make_unique<Impl>(*this); }

	virtual void check(const CheckCallbacks* check_callbacks) const override;

	virtual void save(xmlNode* node) const override;

	virtual void add_create_actions(Actiongraph::Impl& actiongraph) const override;
	virtual void add_modify_actions(Actiongraph::Impl& actiongraph, const Device* lhs) const override;
	virtual void add_delete_actions(Actiongraph::Impl& actiongraph) const override;

	virtual bool equal(const Device::Impl& rhs) const override;
	virtual void log_diff(std::ostream& log, const Device::Impl& rhs_base) const override;
	virtual void print(std::ostream& out) const override;

	virtual Text do_create_text(Tense tense) const override;

	virtual Text do_delete_text(Tense tense) const override;

	virtual Text do_resize_text(const CommitData& commit_data, const Action::Resize* action) const override;
	virtual void do_resize(const CommitData& commit_data, const Action::Resize* action) const override;

	virtual Text do_activate_text(Tense tense) const override;

	virtual Text do_deactivate_text(Tense tense) const override;

	virtual Text do_add_to_etc_crypttab_text(Tense tense) const;
	virtual void do_add_to_etc_crypttab(CommitData& commit_data) const;

	virtual Text do_rename_in_etc_crypttab_text(const Device* lhs, Tense tense) const;
	virtual void do_rename_in_etc_crypttab(CommitData& commit_data) const;

	virtual Text do_remove_from_etc_crypttab_text(Tense tense) const;
	virtual void do_remove_from_etc_crypttab(CommitData& commit_data) const;

    protected:

	/**
	 * Returns the next free name for automatic naming of
	 * encryption devices. It is guaranteed that the name does not
	 * exist in the system and that the same name is never
	 * returned twice.
	 */
	static string next_free_cr_auto_name(const Storage& storage, SystemInfo::Impl& system_info);

	void add_key_file_option_and_execute(const string& cmd_line) const;

    private:

	EncryptionType type = EncryptionType::LUKS1;

	/**
	 * The password.
	 *
	 * Note: The password can be empty. At least for LUKS that does work.
	 */
	string password;

	string key_file;
	bool use_key_file_in_commit = true;

	string cipher;
	unsigned int key_size = 0;
	string pbkdf;
	string integrity;

	MountByType mount_by = MountByType::DEVICE;

	CryptOpts crypt_options;

	bool in_etc_crypttab = true;

	string crypttab_blk_device_name; // block device name as found in /etc/crypttab

	string open_options;

    };

}

#endif

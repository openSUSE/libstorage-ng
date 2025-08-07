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


#include "storage/Devices/EncryptionImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    string
    get_encryption_type_name(EncryptionType encryption_type)
    {
	return toString(encryption_type);
    }


    Encryption*
    Encryption::create(Devicegraph* devicegraph, const string& dm_table_name)
    {
	shared_ptr<Encryption> encryption = make_shared<Encryption>(new Encryption::Impl(dm_table_name));
	Device::Impl::create(devicegraph, encryption);
	return encryption.get();
    }


    Encryption*
    Encryption::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	shared_ptr<Encryption> encryption = make_shared<Encryption>(new Encryption::Impl(node));
	Device::Impl::load(devicegraph, encryption);
	return encryption.get();
    }


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    MountByType
    Encryption::get_mount_by() const
    {
	return get_impl().get_mount_by();
    }


    void
    Encryption::set_mount_by(MountByType mount_by)
    {
	get_impl().set_mount_by(mount_by);
    }


    void
    Encryption::set_default_mount_by()
    {
	get_impl().set_default_mount_by();
    }


    const vector<string>&
    Encryption::get_crypt_options() const
    {
	return get_impl().get_crypt_options().get_opts();;
    }


    void
    Encryption::set_crypt_options(const vector<string>& crypt_options)
    {
	get_impl().set_crypt_options(crypt_options);
    }


    bool
    Encryption::is_in_etc_crypttab() const
    {
	return get_impl().is_in_etc_crypttab();
    }


    void
    Encryption::set_in_etc_crypttab(bool in_etc_crypttab)
    {
	get_impl().set_in_etc_crypttab(in_etc_crypttab);
    }


    Encryption*
    Encryption::clone() const
    {
	return new Encryption(get_impl().clone());
    }


    Encryption::Impl&
    Encryption::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Encryption::Impl&
    Encryption::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    EncryptionType
    Encryption::get_type() const
    {
	return get_impl().get_type();
    }


    void
    Encryption::set_type(EncryptionType type)
    {
	get_impl().set_type(type);
    }


    const string&
    Encryption::get_password() const
    {
	return get_impl().get_password();
    }


    void
    Encryption::set_password(const string& password)
    {
	get_impl().set_password(password);
    }


    const string&
    Encryption::get_key_file() const
    {
	return get_impl().get_key_file();
    }


    void
    Encryption::set_key_file(const string& key_file)
    {
	get_impl().set_key_file(key_file);
    }


    bool
    Encryption::is_use_key_file_in_commit() const
    {
	return get_impl().is_use_key_file_in_commit();
    }


    void
    Encryption::set_use_key_file_in_commit(bool use_key_file_in_commit)
    {
	get_impl().set_use_key_file_in_commit(use_key_file_in_commit);
    }


    const string&
    Encryption::get_cipher() const
    {
	return get_impl().get_cipher();
    }


    void
    Encryption::set_cipher(const string& cipher)
    {
	get_impl().set_cipher(cipher);
    }


    unsigned int
    Encryption::get_key_size() const
    {
	return get_impl().get_key_size();
    }


    void
    Encryption::set_key_size(unsigned int key_size)
    {
	get_impl().set_key_size(key_size);
    }


    const string&
    Encryption::get_pbkdf() const
    {
	return get_impl().get_pbkdf();
    }


    void
    Encryption::set_pbkdf(const std::string& pbkdf)
    {
	get_impl().set_pbkdf(pbkdf);
    }


    const string&
    Encryption::get_integrity() const
    {
	return get_impl().get_integrity();
    }


    void
    Encryption::set_integrity(const std::string& integrity)
    {
	get_impl().set_integrity(integrity);
    }


    BlkDevice*
    Encryption::get_blk_device()
    {
	return get_impl().get_blk_device();
    }


    const BlkDevice*
    Encryption::get_blk_device() const
    {
	return get_impl().get_blk_device();
    }


    const string&
    Encryption::get_open_options() const
    {
	return get_impl().get_open_options();
    }


    void
    Encryption::set_open_options(const string& open_options)
    {
	get_impl().set_open_options(open_options);
    }


    vector<Encryption*>
    Encryption::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<Encryption>();
    }


    vector<const Encryption*>
    Encryption::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const Encryption>();
    }


    bool
    is_encryption(const Device* device)
    {
	return is_device_of_type<const Encryption>(device);
    }


    Encryption*
    to_encryption(Device* device)
    {
	return to_device_of_type<Encryption>(device);
    }


    const Encryption*
    to_encryption(const Device* device)
    {
	return to_device_of_type<const Encryption>(device);
    }

}

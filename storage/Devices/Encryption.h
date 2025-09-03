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


#ifndef STORAGE_ENCRYPTION_H
#define STORAGE_ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    enum class EncryptionType {
	NONE, TWOFISH, TWOFISH_OLD, TWOFISH256_OLD, LUKS, LUKS1 = LUKS, UNKNOWN, LUKS2, PLAIN,
	BITLOCKER
    };


    /**
     * Convert the EncryptionType encryption_type to a string.
     *
     * @see EncryptionType
     */
    std::string get_encryption_type_name(EncryptionType encryption_type);


    // TODO depending on the encryption types supported the Encryption class
    // can be dropped or be abstract


    /**
     * An encryption layer on a blk device
     */
    class Encryption : public BlkDevice
    {
    public:

	/**
	 * Create a device of type Encryption. Usually this function is not called
	 * directly. Instead BlkDevice::create_encryption() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Encryption* create(Devicegraph* devicegraph, const std::string& dm_table_name);

	static Encryption* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get the encryption type.
	 */
	EncryptionType get_type() const;

	/**
	 * Set the encryption type. So far only LUKS1, LUKS2 and PLAIN
	 * are allowed but it is not supported to switch between LUKS
	 * and PLAIN.
	 */
	void set_type(EncryptionType type);

	/**
	 * Get the encryption password.
	 */
	const std::string& get_password() const;

	/**
	 * Set the encryption password.
	 */
	void set_password(const std::string& password);

	/**
	 * Get the key file.
	 */
	const std::string& get_key_file() const;

	/**
	 * Set the key file.
	 *
	 * Usually the key file is inserted in /etc/crypttab and used in commit.
	 *
	 * When accessing the key file the rootprefix is not used. The key file is not
	 * created by libstorage-ng. Thus the key file can be a special device,
	 * e.g. /dev/urandom for swap.
	 *
	 * @see set_use_key_file_in_commit(bool)
	 */
	void set_key_file(const std::string& key_file);

	/**
	 * Get whether the key file is used in commit.
	 */
	bool is_use_key_file_in_commit() const;

	/**
	 * Set whether the key file is used in commit.
	 *
	 * Default is true. The key file must also be set to be used. If set to false the
	 * password must be set.
	 */
	void set_use_key_file_in_commit(bool use_key_file_in_commit);

	/**
	 * Get the cipher.
	 *
	 * Currently only supported for LUKS.
	 */
	const std::string& get_cipher() const;

	/**
	 * Set the cipher. If the cipher is empty the default of
	 * cryptsetup will be used during creation.
	 *
	 * Currently only supported for LUKS.
	 */
	void set_cipher(const std::string& cipher);

	/**
	 * Get the key size in bytes.
	 *
	 * Currently only supported for LUKS.
	 */
	unsigned int get_key_size() const;

	/**
	 * Set the key size in bytes. If the key size is zero the
	 * default of cryptsetup will be used during creation.
	 *
	 * Currently only supported for LUKS.
	 */
	void set_key_size(unsigned int key_size);

	/**
	 * Get the PBKDF (of the first used keyslot).
	 *
	 * Currently only supported for LUKS2.
	 */
	const std::string& get_pbkdf() const;

	/**
	 * Set the PBKDF. Only used during creation. If empty the default of cryptsetup
	 * will be used.
	 *
	 * Currently only supported for LUKS2.
	 */
	void set_pbkdf(const std::string& pbkdf);

	/**
	 * Get the integrity. For now we only expect AEAD.
	 *
	 * Currently only supported for LUKS2 (experimental).
	 */
	const std::string& get_integrity() const;

	/**
	 * Set the integrity. If the integrity is set, this will
	 * restrict the number of allowed cipher algorithms.  Also the
	 * expected value is "aead" or empty string.  Technically this
	 * can be replaced as a boolean flag, but we maintain a string
	 * in case of future new options.  The value is only used
	 * during creation.
	 *
	 * Currently only supported for LUKS2 (experimental).
	 */
	void set_integrity(const std::string& integrity);

	/**
	 * Get the mount-by method. For encrypted devices the mount-by method
	 * defines the name used for the second parameter in /etc/crypttab.
	 */
	MountByType get_mount_by() const;

	/**
	 * Set the mount-by method. For encrypted devices the mount-by method
	 * defines the name used for the second parameter in /etc/crypttab.
	 */
	void set_mount_by(MountByType mount_by);

	/**
	 * Set the mount-by method to the global default, see
	 * Storage::get_default_mount_by(). For encrypted devices the mount-by
	 * method defines the name used for the second parameter in
	 * /etc/crypttab.
	 */
	void set_default_mount_by();

	/**
	 * Get options (fourth field) in /etc/crypttab.
	 */
	const std::vector<std::string>& get_crypt_options() const;

	/**
	 * Set options (fourth field) in /etc/crypttab.
	 */
	void set_crypt_options(const std::vector<std::string>& crypt_options);

	/**
	 * Query whether the LUKS device is present (probed devicegraph) or
	 * will be present (staging devicegraph) in /etc/crypttab.
	 */
	bool is_in_etc_crypttab() const;

	/**
	 * Set whether the LUKS device will be present in /etc/crypttab.
	 */
	void set_in_etc_crypttab(bool in_etc_crypttab);

	/**
	 * Return underlying blk device.
	 *
	 * @throw Exception
	 */
	BlkDevice* get_blk_device();

	/**
	 * @copydoc get_blk_device()
	 */
	const BlkDevice* get_blk_device() const;

	/**
	 * Get extra options for open calls.
	 */
	const std::string& get_open_options() const;

	/**
	 * Set extra options for open calls. The options are
	 * injected as-is to the command so must be properly quoted.
	 */
	void set_open_options(const std::string& open_options);

	/**
	 * Get all Encryption objects of the devicegraph.
	 */
	static std::vector<Encryption*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Encryption*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Encryption* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Encryption(Impl* impl);
	ST_NO_SWIG Encryption(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an Encryption.
     *
     * @throw NullPointerException
     */
    bool is_encryption(const Device* device);

    /**
     * Converts pointer to Device to pointer to Encryption.
     *
     * @return Pointer to Encryption.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Encryption* to_encryption(Device* device);

    /**
     * @copydoc to_encryption(Device*)
     */
    const Encryption* to_encryption(const Device* device);

}

#endif

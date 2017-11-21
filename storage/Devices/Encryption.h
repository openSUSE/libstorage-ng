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


#ifndef STORAGE_ENCRYPTION_H
#define STORAGE_ENCRYPTION_H


#include "storage/Devices/BlkDevice.h"


namespace storage
{

    enum class EncryptionType {
	NONE, TWOFISH, TWOFISH_OLD, TWOFISH256_OLD, LUKS, UNKNOWN
    };


    // TODO depending on the encryption types supported the Encryption class
    // can be dropped or be abstract


    /**
     * An encryption layer on a blk device
     */
    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(Devicegraph* devicegraph, const std::string& name);
	static Encryption* load(Devicegraph* devicegraph, const xmlNode* node);

	EncryptionType get_type() const;

	const std::string& get_password() const;

	void set_password(const std::string& password);

	/**
	 * Get the mount-by method. For encrypted devices the mount-by method
	 * defines the name used for the second parameter in /etc/crypttab.
	 */
	MountByType get_mount_by() const;

	/**
	 * Set the mount-by method. For encrypted devices the mount-by method
	 * defines the name used for the second parameter in /etc/crypttab.
	 */
	void set_mount_by(const MountByType mount_by);

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
	 */
	const BlkDevice* get_blk_device() const;

	static std::vector<Encryption*> get_all(Devicegraph* devicegraph);
	static std::vector<const Encryption*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Encryption* clone() const override;

    protected:

	Encryption(Impl* impl);

    };


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

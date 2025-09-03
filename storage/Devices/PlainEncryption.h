/*
 * Copyright (c) [2019-2020] SUSE LLC
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


#ifndef STORAGE_PLAIN_ENCRYPTION_H
#define STORAGE_PLAIN_ENCRYPTION_H


#include "storage/Devices/Encryption.h"


namespace storage
{

    /**
     * An plain encryption layer on a block device.
     *
     * Only use for swap with a key file (/dev/urandom) is supported.
     *
     * Do not resize the PlainEncryption object but the underlying BlkDevice object.
     */
    class PlainEncryption : public Encryption
    {
    public:

	/**
	 * Create a device of type PlainEncryption. Usually this function is not called
	 * directly. Instead BlkDevice::create_encryption() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static PlainEncryption* create(Devicegraph* devicegraph, const std::string& dm_table_name);

	static PlainEncryption* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all PlainEncryptiones.
	 */
	static std::vector<PlainEncryption*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const PlainEncryption*> get_all(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual PlainEncryption* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	PlainEncryption(Impl* impl);
	ST_NO_SWIG PlainEncryption(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a PlainEncryption.
     *
     * @throw NullPointerException
     */
    bool is_plain_encryption(const Device* device);

    /**
     * Converts pointer to Device to pointer to PlainEncryption.
     *
     * @return Pointer to PlainEncryption.
     * @throw DeviceHasWrongType, NullPointerException
     */
    PlainEncryption* to_plain_encryption(Device* device);

    /**
     * @copydoc to_plain_encryption(Device*)
     */
    const PlainEncryption* to_plain_encryption(const Device* device);

}

#endif

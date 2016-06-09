/*
 * Copyright (c) 2016 SUSE LLC
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


    //! An encryption layer on a device
    class Encryption : public BlkDevice
    {
    public:

	static Encryption* create(Devicegraph* devicegraph, const std::string& name);
	static Encryption* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Return device-mapper table name (dm-name for short).
	 */
	const std::string& get_dm_name() const;

	/**
	 * Return underlying blk device.
	 */
	const BlkDevice* get_blk_device() const;

	void set_dm_name(const std::string& dm_name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Encryption* clone() const override;

    protected:

	Encryption(Impl* impl);

    };


    bool is_encryption(const Device* device);

    Encryption* to_encryption(Device* device);
    const Encryption* to_encryption(const Device* device);

}

#endif

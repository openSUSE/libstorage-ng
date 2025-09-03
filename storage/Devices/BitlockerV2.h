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


#ifndef STORAGE_BITLOCKER_V2_H
#define STORAGE_BITLOCKER_V2_H


#include "storage/Devices/Encryption.h"


namespace storage
{

    /**
     * A BitLocker layer on a block device.
     *
     * Uses cryptsetup. Generated during probing if
     * LIBSTORAGE_CRYPTSETUP_FOR_BITLOCKER=yes.
     *
     * The block device is likely only usable by the NTFS that is already on it since the
     * device-mapper table contains or may contain "zero" mappings.
     */
    class BitlockerV2 : public Encryption
    {
    public:

	/**
	 * Create a device of type BitlockerV2. Usually this function is not called
	 * directly. Instead BlkDevice::create_encryption() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static BitlockerV2* create(Devicegraph* devicegraph, const std::string& dm_table_name);

	static BitlockerV2* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all BitlockerV2s.
	 */
	static std::vector<BitlockerV2*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BitlockerV2*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get the BitLocker UUID.
	 */
	const std::string& get_uuid() const;

	/**
	 * The library keeps track of whether the activation of a specific BitLocker
	 * device was canceled and of the password. Using this function that information
	 * can be reset.
	 */
	static void reset_activation_infos();

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual BitlockerV2* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	BitlockerV2(Impl* impl);
	ST_NO_SWIG BitlockerV2(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a BitlockerV2.
     *
     * @throw NullPointerException
     */
    bool is_bitlocker_v2(const Device* device);

    /**
     * Converts pointer to Device to pointer to BitlockerV2.
     *
     * @return Pointer to BitlockerV2.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BitlockerV2* to_bitlocker_v2(Device* device);

    /**
     * @copydoc to_bitlocker_v2(Device*)
     */
    const BitlockerV2* to_bitlocker_v2(const Device* device);

}

#endif

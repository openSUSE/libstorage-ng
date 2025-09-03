/*
 * Copyright (c) [2016-2021] SUSE LLC
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


#ifndef STORAGE_LUKS_H
#define STORAGE_LUKS_H


#include "storage/Devices/Encryption.h"


namespace storage
{

    /**
     * An LUKS encryption layer on a block device.
     *
     * Do not resize the Luks object but the underlying BlkDevice object.
     */
    class Luks : public Encryption
    {
    public:

	/**
	 * Create a device of type Luks. Usually this function is not called
	 * directly. Instead BlkDevice::create_encryption() is called.
	 *
	 * @see Device::create(Devicegraph*)
	 */
	static Luks* create(Devicegraph* devicegraph, const std::string& dm_table_name);

	static Luks* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Lukses.
	 */
	static std::vector<Luks*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Luks*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get the LUKS UUID.
	 */
	const std::string& get_uuid() const;

	/**
	 * Set the LUKS UUID. The UUID is only set when creating a new LUKS on disk.
	 */
	void set_uuid(const std::string& uuid);

	/**
	 * Get the LUKS label.
	 *
	 * Only available in LUKS version 2.
	 */
	const std::string& get_label() const;

	/**
	 * Set the LUKS label.
	 *
	 * Only available in LUKS version 2.
	 */
	void set_label(const std::string& label);

	/**
	 * Get extra options for luks format call.
	 */
	const std::string& get_format_options() const;

	/**
	 * Set extra options for luks format call. The options are
	 * injected as-is to the command so must be properly quoted.
	 *
	 * Options that modify the size of the resulting blk device
	 * (e.g. --integrity) are not allowed.
	 */
	void set_format_options(const std::string& format_options);

	/**
	 * The library keeps track of whether the activation of a specific LUKS device was
	 * canceled and of the password. Using this function that information can be
	 * reset.
	 */
	static void reset_activation_infos();

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Luks* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	Luks(Impl* impl);
	ST_NO_SWIG Luks(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a Luks.
     *
     * @throw NullPointerException
     */
    bool is_luks(const Device* device);

    /**
     * Converts pointer to Device to pointer to Luks.
     *
     * @return Pointer to Luks.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Luks* to_luks(Device* device);

    /**
     * @copydoc to_luks(Device*)
     */
    const Luks* to_luks(const Device* device);

}

#endif

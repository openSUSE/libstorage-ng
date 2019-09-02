/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2019] SUSE LLC
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


#ifndef STORAGE_DISK_H
#define STORAGE_DISK_H


#include "storage/Devices/Partitionable.h"


namespace storage
{

    /**
     * Data Transport Layer
     */
    enum class Transport {
	UNKNOWN, SBP, ATA, FC, ISCSI, SAS, SATA, SPI, USB, FCOE
    };


    /**
     * Convert the Transport transport to a string.
     */
    std::string get_transport_name(Transport transport);


    /**
     * Zone model as read from /sys. See e.g. https://www.zonedstorage.io/ and
     * https://github.com/torvalds/linux/blob/master/Documentation/ABI/testing/sysfs-block
     * for further information.
     */
    enum class ZoneModel {
	NONE, HOST_AWARE, HOST_MANAGED
    };


    /**
     * Convert the ZoneModel zone_model to a string.
     */
    std::string get_zone_model_name(ZoneModel zone_model);


    //! A physical disk device
    class Disk : public Partitionable
    {
    public:

	static Disk* create(Devicegraph* devicegraph, const std::string& name);
	static Disk* create(Devicegraph* devicegraph, const std::string& name,
			    const Region& region);
	static Disk* create(Devicegraph* devicegraph, const std::string& name,
			    unsigned long long size);

	static Disk* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Get all Disks.
	 */
	static std::vector<Disk*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Disk*> get_all(const Devicegraph* devicegraph);

	bool is_rotational() const;

	/**
	 * Get the transport of the disk.
	 */
	Transport get_transport() const;

	/**
	 * Get the zone model of the disk.
	 */
	ZoneModel get_zone_model() const;

	/**
	 * Find a Disk by its name. Only the name returned by get_name() is
	 * considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Disk* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Disk* find_by_name(const Devicegraph* devicegraph, const std::string& name);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Disk* clone() const override;

    protected:

	Disk(Impl* impl);

    };


    /**
     * Checks whether device points to a Disk.
     *
     * @throw NullPointerException
     */
    bool is_disk(const Device* device);

    /**
     * Converts pointer to Device to pointer to Disk.
     *
     * @return Pointer to Disk.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Disk* to_disk(Device* device);

    /**
     * @copydoc to_disk(Device*)
     */
    const Disk* to_disk(const Device* device);

}

#endif

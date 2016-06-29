/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_BLK_DEVICE_H
#define STORAGE_BLK_DEVICE_H


#include "storage/Devices/Device.h"
#include "storage/Devices/Filesystem.h"


namespace storage
{

    class Region;


    //! An abstract Block Device.
    class BlkDevice : public Device
    {
    public:

	virtual void check() const override;

	const std::string& get_name() const;
	void set_name(const std::string& name);

	const Region& get_region() const;
	void set_region(const Region& region);

	unsigned long long get_size() const;
	void set_size(unsigned long long size);

	std::string get_size_string() const;

	const std::string& get_sysfs_name() const;
	const std::string& get_sysfs_path() const;

	const std::string& get_udev_path() const;
	const std::vector<std::string>& get_udev_ids() const;

	ResizeInfo detect_resize_info() const;

	static std::vector<BlkDevice*> get_all(Devicegraph* devicegraph);
	static std::vector<const BlkDevice*> get_all(const Devicegraph* devicegraph);

	static BlkDevice* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const BlkDevice* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	Filesystem* create_filesystem(FsType fs_type);

	bool has_filesystem() const;

	Filesystem* get_filesystem();
	const Filesystem* get_filesystem() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkDevice(Impl* impl);

    };


    bool is_blk_device(const Device* device);

    BlkDevice* to_blk_device(Device* device);
    const BlkDevice* to_blk_device(const Device* device);

}

#endif

/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#include "storage/Utils/StorageTmpl.h"
#include "storage/Filesystems/BlkFilesystemImpl.h"
#include "storage/Devicegraph.h"
#include "storage/FindBy.h"


namespace storage
{

    using namespace std;


    BlkFilesystem::BlkFilesystem(Impl* impl)
	: Filesystem(impl)
    {
    }


    BlkFilesystem::BlkFilesystem(unique_ptr<Device::Impl>&& impl)
       : Filesystem(std::move(impl))
    {
    }


    BlkFilesystem::Impl&
    BlkFilesystem::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const BlkFilesystem::Impl&
    BlkFilesystem::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<BlkFilesystem*>
    BlkFilesystem::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<BlkFilesystem>();
    }


    vector<const BlkFilesystem*>
    BlkFilesystem::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const BlkFilesystem>();
    }


    bool
    BlkFilesystem::supports_label() const
    {
	return get_impl().supports_label();
    }


    unsigned int
    BlkFilesystem::max_labelsize() const
    {
	return get_impl().max_labelsize();
    }


    const string&
    BlkFilesystem::get_label() const
    {
	return get_impl().get_label();
    }


    void
    BlkFilesystem::set_label(const string& label)
    {
	get_impl().set_label(label);
    }


    bool
    BlkFilesystem::supports_uuid() const
    {
	return get_impl().supports_uuid();
    }


    const string&
    BlkFilesystem::get_uuid() const
    {
	return get_impl().get_uuid();
    }


    void
    BlkFilesystem::set_uuid(const string& uuid)
    {
	get_impl().set_uuid(uuid);
    }


    const string&
    BlkFilesystem::get_mkfs_options() const
    {
	return get_impl().get_mkfs_options();
    }


    void
    BlkFilesystem::set_mkfs_options(const string& mkfs_options)
    {
	get_impl().set_mkfs_options(mkfs_options);
    }


    bool
    BlkFilesystem::supports_tune_options() const
    {
	return get_impl().supports_tune_options();
    }


    const string&
    BlkFilesystem::get_tune_options() const
    {
	return get_impl().get_tune_options();
    }


    void
    BlkFilesystem::set_tune_options(const string& tune_options)
    {
	get_impl().set_tune_options(tune_options);
    }


    bool
    BlkFilesystem::supports_shrink() const
    {
	return get_impl().supports_shrink();
    }


    bool
    BlkFilesystem::supports_grow() const
    {
	return get_impl().supports_grow();
    }


    bool
    BlkFilesystem::supports_mounted_shrink() const
    {
	return get_impl().supports_mounted_shrink();
    }


    bool
    BlkFilesystem::supports_mounted_grow() const
    {
	return get_impl().supports_mounted_grow();
    }


    bool
    BlkFilesystem::supports_unmounted_shrink() const
    {
	return get_impl().supports_unmounted_shrink();
    }


    bool
    BlkFilesystem::supports_unmounted_grow() const
    {
	return get_impl().supports_unmounted_grow();
    }


    void
    BlkFilesystem::set_resize_info(const ResizeInfo& resize_info)
    {
	get_impl().set_resize_info(resize_info);
    }


    ContentInfo
    BlkFilesystem::detect_content_info() const
    {
	return get_impl().detect_content_info();
    }


    void
    BlkFilesystem::set_content_info(const ContentInfo& content_info)
    {
	get_impl().set_content_info(content_info);
    }


    vector<const BlkFilesystem*>
    BlkFilesystem::find_by_label(const Devicegraph* devicegraph, const string& label)
    {
	auto pred = [&label](const BlkFilesystem* blk_filesystem) {
	    return blk_filesystem->get_label() == label;
	};

	return devicegraph->get_impl().get_devices_of_type_if<const BlkFilesystem>(pred);
    }


    vector<const BlkFilesystem*>
    BlkFilesystem::find_by_uuid(const Devicegraph* devicegraph, const string& uuid)
    {
	auto pred = [&uuid](const BlkFilesystem* blk_filesystem) {
	    return blk_filesystem->get_uuid() == uuid;
	};

	return devicegraph->get_impl().get_devices_of_type_if<const BlkFilesystem>(pred);
    }


    vector<BlkDevice*>
    BlkFilesystem::get_blk_devices()
    {
	return get_impl().get_blk_devices();
    }


    vector<const BlkDevice*>
    BlkFilesystem::get_blk_devices() const
    {
	return get_impl().get_blk_devices();
    }


    bool
    is_blk_filesystem(const Device* device)
    {
	return is_device_of_type<const BlkFilesystem>(device);
    }


    BlkFilesystem*
    to_blk_filesystem(Device* device)
    {
	return to_device_of_type<BlkFilesystem>(device);
    }


    const BlkFilesystem*
    to_blk_filesystem(const Device* device)
    {
	return to_device_of_type<const BlkFilesystem>(device);
    }

}

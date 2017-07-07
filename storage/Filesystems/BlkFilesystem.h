/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_BLK_FILESYSTEM_H
#define STORAGE_BLK_FILESYSTEM_H


#include <vector>

#include "storage/Filesystems/Filesystem.h"


namespace storage
{

    class BlkDevice;
    class ContentInfo;


    // abstract class

    class BlkFilesystem : public Filesystem
    {
    public:

	static std::vector<BlkFilesystem*> get_all(Devicegraph* devicegraph);
	static std::vector<const BlkFilesystem*> get_all(const Devicegraph* devicegraph);

	bool supports_label() const;
	unsigned int max_labelsize() const;

	const std::string& get_label() const;
	void set_label(const std::string& label);

	bool supports_uuid() const;

	const std::string& get_uuid() const;
	void set_uuid(const std::string& uuid);

	const std::string& get_mkfs_options() const;
	void set_mkfs_options(const std::string& mkfs_options);

	const std::string& get_tune_options() const;
	void set_tune_options(const std::string& tune_options);

	/**
	 * Set the ResizeInfo. Only use for testsuites.
	 */
	void set_resize_info(const ResizeInfo& resize_info);

	ContentInfo detect_content_info() const;

	/**
	 * Set the ResizeInfo. Only use for testsuites.
	 */
	void set_content_info(const ContentInfo& content_info);

	static std::vector<const BlkFilesystem*> find_by_label(const Devicegraph* devicegraph,
							       const std::string& label);

	static std::vector<const BlkFilesystem*> find_by_uuid(const Devicegraph* devicegraph,
							      const std::string& uuid);

	std::vector<const BlkDevice*> get_blk_devices() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkFilesystem(Impl* impl);

    };


    bool is_blk_filesystem(const Device* device);

    /**
     * Converts pointer to Device to pointer to BlkFilesystem.
     *
     * @return Pointer to BlkFilesystem.
     * @throw DeviceHasWrongType, NullPointerException
     */
    BlkFilesystem* to_blk_filesystem(Device* device);

    /**
     * @copydoc to_blk_filesystem(Device*)
     */
    const BlkFilesystem* to_blk_filesystem(const Device* device);

}

#endif

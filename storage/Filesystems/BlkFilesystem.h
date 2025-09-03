/*
 * Copyright (c) [2014-2015] Novell, Inc.
 * Copyright (c) [2016-2022] SUSE LLC
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

	/**
	 * Get all BlkFilesystems.
	 */
	static std::vector<BlkFilesystem*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const BlkFilesystem*> get_all(const Devicegraph* devicegraph);

	/**
	 * Return whether the filesystem supports a label.
	 */
	bool supports_label() const;

	/**
	 * Return the maximal allowed filesystem label length.
	 */
	unsigned int max_labelsize() const;

	/**
	 * Get the filesystem label.
	 */
	const std::string& get_label() const;

	/**
	 * Set the filesystem label. May not work if the filesystem is mounted (e.g. for
	 * swap).
	 */
	void set_label(const std::string& label);

	/**
	 * Return whether the filesystem supports a UUID.
	 */
	bool supports_uuid() const;

	/**
	 * Get the filesystem UUID. May be empty.
	 */
	const std::string& get_uuid() const;

	/**
	 * Set the filesystem UUID. Only supported for btrfs, xfs, ext, reiserfs, jfs,
	 * udf, nilfs2 and swap. In general the UUID is only set when creating a new
	 * filesystem on disk. May not work if the filesystem is mounted (e.g. for swap).
	 */
	void set_uuid(const std::string& uuid);

	/**
	 * Get extra options for the filesystem mkfs command.
	 */
	const std::string& get_mkfs_options() const;

	/**
	 * Set extra options for the filesystem mkfs command. The
	 * options are injected as-is to the command so must be
	 * properly quoted.
	 */
	void set_mkfs_options(const std::string& mkfs_options);

	/**
	 * Checks whether the filesystem supports tune options.
	 */
	bool supports_tune_options() const;

	/**
	 * Get extra options for the filesystem tune command.
	 */
	const std::string& get_tune_options() const;

	/**
	 * Set extra options for the filesystem tune command. The
	 * options are injected as-is to the command so must be
	 * properly quoted. May not work if the filesystem is mounted.
	 *
	 * Only supported on Ext, Reiserfs and Nilfs2.
	 */
	void set_tune_options(const std::string& tune_options);

	/**
	 * Checks whether shrinking the filesystem is supported.
	 */
	bool supports_shrink() const;

	/**
	 * Checks whether growing the filesystem is supported.
	 */
	bool supports_grow() const;

	/**
	 * Checks whether shrinking the filesystem while being mounted is
	 * supported.
	 */
	bool supports_mounted_shrink() const;

	/**
	 * Checks whether growing the filesystem while being mounted is
	 * supported.
	 */
	bool supports_mounted_grow() const;

	/**
	 * Checks whether shrinking the filesystem while being unmounted is
	 * supported.
	 */
	bool supports_unmounted_shrink() const;

	/**
	 * Checks whether growing the filesystem while being unmounted is
	 * supported.
	 */
	bool supports_unmounted_grow() const;

	/**
	 * Set the ResizeInfo. Only use for testsuites.
	 */
	void set_resize_info(const ResizeInfo& resize_info);

	/**
	 * Detect the content info of the filesystem.
	 *
	 * @throw Exception
	 */
	ContentInfo detect_content_info() const;

	/**
	 * Set the ResizeInfo. Only use for testsuites.
	 */
	void set_content_info(const ContentInfo& content_info);

	/**
	 * Find filesystems by label.
	 */
	static std::vector<const BlkFilesystem*> find_by_label(const Devicegraph* devicegraph,
							       const std::string& label);

	/**
	 * Find filesystems by UUID.
	 */
	static std::vector<const BlkFilesystem*> find_by_uuid(const Devicegraph* devicegraph,
							      const std::string& uuid);

	/**
	 * Get underlying blk devices of the blk filesystem.
	 */
	std::vector<BlkDevice*> get_blk_devices();

	/**
	 * @copydoc get_blk_devices()
	 */
	std::vector<const BlkDevice*> get_blk_devices() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	BlkFilesystem(Impl* impl);
	ST_NO_SWIG BlkFilesystem(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a BlkFilesystem.
     *
     * @throw NullPointerException
     */
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

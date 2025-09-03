/*
 * Copyright (c) [2016-2020] SUSE LLC
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


#ifndef STORAGE_FILESYSTEM_USER_H
#define STORAGE_FILESYSTEM_USER_H


#include "storage/Holders/User.h"


namespace storage
{

    /**
     * Holder from BlkDevice to a BlkFilesystem.
     *
     * @see User, Holder
     */
    class FilesystemUser : public User
    {
    public:

	/**
	 * Create a holder of type FilesystemUser. Usually this function is not called
	 * directly. Functions like BlkDevice::create_blk_filesystem() or
	 * Btrfs::add_device() call it.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static FilesystemUser* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static FilesystemUser* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual FilesystemUser* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

	/**
	 * Indicates whether the block device is used as an external journal device.
	 */
	bool is_journal() const;

	/**
	 * Set the external journal flag. The library cannot create filesystem with
	 * external journals.
	 *
	 * @see is_journal()
	 */
	void set_journal(bool journal);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	FilesystemUser(Impl* impl);
	ST_NO_SWIG FilesystemUser(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to a FilesystemUser.
     *
     * @throw NullPointerException
     */
    bool is_filesystem_user(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to FilesystemUser.
     *
     * @return Pointer to FilesystemUser.
     * @throw HolderHasWrongType, NullPointerException
     */
    FilesystemUser* to_filesystem_user(Holder* holder);

    /**
     * @copydoc to_filesystem_user(Holder*)
     */
    const FilesystemUser* to_filesystem_user(const Holder* holder);

}

#endif

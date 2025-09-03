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


#ifndef STORAGE_MD_USER_H
#define STORAGE_MD_USER_H


#include "storage/Holders/User.h"


namespace storage
{

    /**
     * Holder from a BlkDevice to an Md.
     *
     * @see User, Holder
     */
    class MdUser : public User
    {
    public:

	/**
	 * Create a holder of type MdUser. Usually this function is not called
	 * directly. Functions like Md::add_device() call it.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static MdUser* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static MdUser* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual MdUser* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

	/**
	 * Return whether the target device is a spare device.
	 */
	bool is_spare() const;

	/**
	 * Set the target device to be a spare device.
	 *
	 * @see is_spare()
	 *
	 * @throw Exception
	 */
	void set_spare(bool spare);

	/**
	 * Return whether the target device is a faulty device.
	 */
	bool is_faulty() const;

	/**
	 * Set the target device to be a faulty device.
	 *
	 * @see is_faulty()
	 *
	 * @throw Exception
	 */
	void set_faulty(bool faulty);

	/**
	 * Return whether the target device is a journal device. Only one device of an MD
	 * RAID can be a journal device. Only for RAID levels 4, 5 and 6.
	 */
	bool is_journal() const;

	/**
	 * Set the target device to be a journal device.
	 *
	 * Changing the value is not supported for RAIDs already created on-disk. Does
	 * not work with metadata version 1.0.
	 *
	 * @see is_journal()
	 *
	 * @throw Exception
	 */
	void set_journal(bool journal);

	/**
	 * Return the sort key.
	 *
	 * When creating a MD RAID the device list passed to the mdadm command is sorted
	 * according to the sort-key. For use-cases see https://fate.suse.com/313521/.
	 *
	 * The RAID devices, spare and journal devices are sorted independently. Sorting
	 * of devices with the same sort-key is undefined. Any value is allowed but 0
	 * should mean unknown/unspecified.
	 *
	 * @see set_sort_key()
	 */
	unsigned int get_sort_key() const;

	/**
	 * Set the sort key.
	 *
	 * @see get_sort_key().
	 */
	void set_sort_key(unsigned int sort_key);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	MdUser(Impl* impl);
	ST_NO_SWIG MdUser(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to a MdUser.
     *
     * @throw NullPointerException
     */
    bool is_md_user(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to MdUser.
     *
     * @return Pointer to MdUser.
     * @throw HolderHasWrongType, NullPointerException
     */
    MdUser* to_md_user(Holder* holder);

    /**
     * @copydoc to_md_user(Holder*)
     */
    const MdUser* to_md_user(const Holder* holder);

}

#endif

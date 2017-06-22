/*
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


#ifndef STORAGE_MD_H
#define STORAGE_MD_H


#include <functional>

#include "storage/Devices/Partitionable.h"


namespace storage
{

    class MdUser;


    enum MdLevel {
	UNKNOWN, RAID0, RAID1, RAID5, RAID6, RAID10
    };

    enum MdParity {
	DEFAULT, LEFT_ASYMMETRIC, LEFT_SYMMETRIC, RIGHT_ASYMMETRIC,
	RIGHT_SYMMETRIC, FIRST, LAST, LEFT_ASYMMETRIC_6, LEFT_SYMMETRIC_6,
	RIGHT_ASYMMETRIC_6, RIGHT_SYMMETRIC_6, FIRST_6, NEAR_2, OFFSET_2,
	FAR_2, NEAR_3, OFFSET_3, FAR_3
    };


    std::string get_md_level_name(MdLevel md_level);

    std::string get_md_parity_name(MdParity md_parity);


    /**
     * A MD device
     */
    class Md : public Partitionable
    {
    public:

	/**
	 * Create a MD in devicegraph with name. Name must either be of the
	 * form "/dev/md<number>" or "/dev/md/<name>". The first form is
	 * called numeric. If name is a number it is also called numeric.
	 */
	static Md* create(Devicegraph* devicegraph, const std::string& name);

	static Md* load(Devicegraph* devicegraph, const xmlNode* node);

	MdUser* add_device(BlkDevice* blk_device);
	void remove_device(BlkDevice* blk_device);

	std::vector<BlkDevice*> get_devices();
	std::vector<const BlkDevice*> get_devices() const;

	/**
	 * Returns true if the name of the MD is numeric.
	 */
	bool is_numeric() const;

	/**
	 * Returns the number of the MD. Throws if the MD is not numeric.
	 */
	unsigned int get_number() const;

	MdLevel get_md_level() const;
	void set_md_level(MdLevel md_level);

	MdParity get_md_parity() const;
	void set_md_parity(MdParity md_parity);

	unsigned long get_chunk_size() const;
	void set_chunk_size(unsigned long chunk_size);

	const std::string& get_uuid() const;

	const std::string& get_superblock_version() const;

	/**
	 * Query whether the MD RAID is present (probed devicegraph) or will
	 * be present (staging devicegraph) in /etc/mdadm.conf.
	 */
	bool is_in_etc_mdadm() const;

	/**
	 * Set whether the MD RAID will be present in /etc/mdadm.conf.
	 */
	void set_in_etc_mdadm(bool in_etc_mdadm);

	/**
	 * Get all Mds. Sorted by name and number: The non-numeric Mds come
	 * before the numeric Mds.
	 */
	static std::vector<Md*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Md*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get all Mds for which the predicate pred returns true. Sorted by
	 * name and number: The non-numeric Mds come before the numeric Mds.
	 */
	static std::vector<Md*> get_all_if(Devicegraph* devicegraph,
					   std::function<bool(const Md*)> pred);

	/**
	 * @copydoc get_all_if()
	 */
	static std::vector<const Md*> get_all_if(const Devicegraph* devicegraph,
						 std::function<bool(const Md*)> pred);

	static Md* find_by_name(Devicegraph* devicegraph, const std::string& name);
	static const Md* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Find a free numeric name for a MD.
	 */
	static std::string find_free_numeric_name(const Devicegraph* devicegraph);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Md* clone() const override;

    protected:

	Md(Impl* impl);

    };


    bool is_md(const Device* device);

    /**
     * Converts pointer to Device to pointer to Md.
     *
     * @return Pointer to Md.
     * @throw DeviceHasWrongType, NullPointerException
     */
    Md* to_md(Device* device);

    /**
     * @copydoc to_md(Device*)
     */
    const Md* to_md(const Device* device);

}

#endif

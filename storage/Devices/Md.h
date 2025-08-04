/*
 * Copyright (c) [2016-2025] SUSE LLC
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


    /**
     * MD RAID levels.
     */
    enum class MdLevel
    {
	UNKNOWN, RAID0, RAID1, RAID4, RAID5, RAID6, RAID10, CONTAINER, LINEAR
    };


    /**
     * MD parity algorithms for RAID5, RAID6 and RAID10.
     */
    enum class MdParity
    {
	DEFAULT, LEFT_ASYMMETRIC, LEFT_SYMMETRIC, RIGHT_ASYMMETRIC,
	RIGHT_SYMMETRIC, FIRST, LAST, LEFT_ASYMMETRIC_6, LEFT_SYMMETRIC_6,
	RIGHT_ASYMMETRIC_6, RIGHT_SYMMETRIC_6, FIRST_6, NEAR_2, OFFSET_2,
	FAR_2, NEAR_3, OFFSET_3, FAR_3
    };


    /**
     * Convert the MD RAID level md_level to a string.
     *
     * @see MdLevel
     */
    std::string get_md_level_name(MdLevel md_level);


    /**
     * Convert the MD parity algorithm md_parity to a string.
     *
     * @see MdParity
     */
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
	 *
	 * Tests have shown that the number must be between 0 and 1048575.
	 *
	 * @see Device::create(Devicegraph*)
	 *
	 * @throw Exception
	 */
	static Md* create(Devicegraph* devicegraph, const std::string& name);

	static Md* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * Check whether the MD name is valid. Does not check for collisions. May depend
	 * on version of system components, e.g. mdadm.
	 *
	 * The name is a complete name including /dev or /dev/md, e.g. "/dev/md0" or
	 * "/dev/md/test".
	 */
	static bool is_valid_name(const std::string& name);

	/**
	 * Add another device to a RAID.
	 *
	 * For combining disks with different block sizes,
	 * see doc/md-raid.md.
	 *
	 * @throw WrongNumberOfChildren
	 */
	MdUser* add_device(BlkDevice* blk_device);

	/**
	 * @throw Exception
	 */
	void remove_device(BlkDevice* blk_device);

	std::vector<BlkDevice*> get_devices() ST_DEPRECATED;
	std::vector<const BlkDevice*> get_devices() const ST_DEPRECATED;

	/**
	 * Return blk devices used for the MD RAID. For Mds of type
	 * MdMember this does not include the MdContainer.
	 */
	std::vector<BlkDevice*> get_blk_devices();

	/**
	 * @copydoc get_blk_devices()
	 */
	std::vector<const BlkDevice*> get_blk_devices() const;

	/**
	 * Returns true iff the name of the MD is numeric. This is the case for
	 * e.g. /dev/md1 and also /dev/md/2.
	 */
	bool is_numeric() const;

	/**
	 * Returns the number of the MD. Throws if the MD is not numeric or if the number
	 * is out of range.
	 *
	 * @throw Exception
	 */
	unsigned int get_number() const;

	/**
	 * Get the MD RAID level.
	 *
	 * @see MdLevel
	 */
	MdLevel get_md_level() const;

	/**
	 * Set the MD RAID level.
	 *
	 * @see MdLevel
	 */
	void set_md_level(MdLevel md_level);

	/**
	 * Get the parity of the MD RAID. Only meaningful for RAID5, RAID6 and
	 * RAID10.
	 *
	 * @see MdParity
	 */
	MdParity get_md_parity() const;

	/**
	 * Set the parity of the MD RAID. Only meaningful for RAID5, RAID6 and
	 * RAID10 and for MD RAIDs not created on disk yet.
	 *
	 * @see MdParity
	 */
	void set_md_parity(MdParity md_parity);

	/**
	 * Get the allowed parities for the MD RAID. Only meaningful for
	 * RAID5, RAID6 and RAID10. So far depends on the MD RAID level and
	 * the number of devices.
	 *
	 * @throw Exception
	 */
	std::vector<MdParity> get_allowed_md_parities() const;

	/**
	 * Is the chunk size meaningful for the RAID (so far only depends on RAID level)?
	 */
	bool is_chunk_size_meaningful() const;

	/**
	 * Get the chunk size of the MD RAID. The chunk size is not meaningful for LINEAR
	 * nor RAID1.
	 *
	 * @see is_chunk_size_meaningful()
	 */
	unsigned long get_chunk_size() const;

	/**
	 * Set the chunk size of the MD RAID. The chunk size is not meaningful for LINEAR
	 * nor RAID1.
	 *
	 * The function does not make a complete check of the chunk size since that
	 * depends on the RAID Level and the underlying devices. Use the
	 * Devicegraph::check() function.
	 *
	 * Only for MD RAIDs not created on disk yet.
	 *
	 * @see is_chunk_size_meaningful()
	 *
	 * @throw InvalidChunkSize, Exception
	 */
	void set_chunk_size(unsigned long chunk_size);

	/**
	 * Get the UUID.
	 *
	 * Note: Special MD RAID format.
	 */
	const std::string& get_uuid() const;

	/**
	 * Set the UUID. The UUID is only set when creating a new RAID on disk.
	 *
	 * Note: Special MD RAID format.
	 */
	void set_uuid(const std::string& uuid);

	/**
	 * A string like "1.0" or "1.2" for Linux RAID, "imsm" or "ddf" for BIOS RAID
	 * containers and empty for BIOS RAID members.
	 */
	const std::string& get_metadata() const;

	/**
	 * Set metadata for new created Linux RAID. Allowed values are "1.0", "1.1", "1.2"
	 * and "default" (as long as mdadm treats "default" as "1.2"). Other values might
	 * also work.
	 *
	 * If a specific metadata version is required it should be set since the default
	 * can change.
	 */
	void set_metadata(const std::string& metadata);

	/**
	 * Return the minimal number of devices required by the RAID (without spare and
	 * journal devices). For RAIDs of level CONTAINER it returns 0 (those RAIDs cannot
	 * be created or modified anyway).
	 */
	unsigned int minimal_number_of_devices() const;

	/**
	 * Return whether the RAID supports spare devices. This is the case for RAID1,
	 * RAID4, RAID5, RAID6 and RAID10.
	 */
	bool supports_spare_devices() const;

	/**
	 * Return whether the RAID supports a journal device. This is the case for RAID4,
	 * RAID5 and RAID6.
	 */
	bool supports_journal_device() const;

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
	 * Get all Mds.
	 */
	static std::vector<Md*> get_all(Devicegraph* devicegraph);

	/**
	 * @copydoc get_all()
	 */
	static std::vector<const Md*> get_all(const Devicegraph* devicegraph);

	/**
	 * Get all Mds for which the predicate pred returns true.
	 */
	static std::vector<Md*> get_all_if(Devicegraph* devicegraph,
					   std::function<bool(const Md*)> pred);

	/**
	 * @copydoc get_all_if()
	 */
	static std::vector<const Md*> get_all_if(const Devicegraph* devicegraph,
						 std::function<bool(const Md*)> pred);

	/**
	 * Find a Md by its name. Only the name returned by get_name() is
	 * considered.
	 *
	 * @throw DeviceNotFound, DeviceHasWrongType
	 */
	static Md* find_by_name(Devicegraph* devicegraph, const std::string& name);

	/**
	 * @copydoc find_by_name
	 */
	static const Md* find_by_name(const Devicegraph* devicegraph, const std::string& name);

	/**
	 * Find a free numeric name for a MD, e.g. "/dev/md2". A free numeric name might
	 * not be available.
	 *
	 * @throw Exception
	 */
	static std::string find_free_numeric_name(const Devicegraph* devicegraph);

	/**
	 * Calculate the required size of the underlying block devices to get an MD of the
	 * desired size.
	 *
	 * Calculation is not accurate. Does not consider details like alignment, chunk
	 * size, bitmap location or metadata version. It may also change in future
	 * versions.
	 *
	 * @throw Exception
	 */
	static unsigned long long calculate_underlying_size(MdLevel md_level, unsigned int number_of_devices,
							    unsigned long long size);

	/**
	 * Compare (less than) two Mds by number.
	 *
	 * @see get_number()
	 *
	 * @throw Exception
	 */
	static bool compare_by_number(const Md* lhs, const Md* rhs);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual Md* clone() const override;

	Md(Impl* impl);

    };


    /**
     * Checks whether device points to a Md.
     *
     * @throw NullPointerException
     */
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

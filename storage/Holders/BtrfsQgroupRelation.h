/*
 * Copyright (c) 2020 SUSE LLC
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


#ifndef STORAGE_BTRFS_QGROUP_RELATION_H
#define STORAGE_BTRFS_QGROUP_RELATION_H


#include "storage/Holders/Holder.h"


namespace storage
{

    class Btrfs;


    /**
     * Relation between btrfs quota groups.
     *
     * Not included in the classic view.
     *
     * @see Holder
     */
    class BtrfsQgroupRelation : public Holder
    {
    public:

	/**
	 * Create a holder of type BtrfsQgroupRelation. Usually this function is not called
	 * directly.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static BtrfsQgroupRelation* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static BtrfsQgroupRelation* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual BtrfsQgroupRelation* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

	/**
	 * Get the btrfs the relation belongs to.
	 */
	const Btrfs* get_btrfs() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	BtrfsQgroupRelation(Impl* impl);
	ST_NO_SWIG BtrfsQgroupRelation(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to a BtrfsQgroupRelation.
     *
     * @throw NullPointerException
     */
    bool is_btrfs_qgroup_relation(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to BtrfsQgroupRelation.
     *
     * @return Pointer to BtrfsQgroupRelation.
     * @throw HolderHasWrongType, NullPointerException
     */
    BtrfsQgroupRelation* to_btrfs_qgroup_relation(Holder* holder);

    /**
     * @copydoc to_btrfs_qgroup_relation(Holder*)
     */
    const BtrfsQgroupRelation* to_btrfs_qgroup_relation(const Holder* holder);

}

#endif

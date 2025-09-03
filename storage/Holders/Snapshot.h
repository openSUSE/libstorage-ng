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


#ifndef STORAGE_SNAPSHOT_H
#define STORAGE_SNAPSHOT_H


#include "storage/Holders/Holder.h"


namespace storage
{

    /**
     * Relationship between origin and snapshot. The origin is the source and the
     * snapshot the target. Used for LVM and btrfs snapshot.
     *
     * Not included in the classic view.
     *
     * @see Holder
     */
    class Snapshot : public Holder
    {
    public:

	/**
	 * Create a holder of type Snapshot. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static Snapshot* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static Snapshot* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual Snapshot* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	Snapshot(Impl* impl);
	ST_NO_SWIG Snapshot(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to a Snapshot.
     *
     * @throw NullPointerException
     */
    bool is_snapshot(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to Snapshot.
     *
     * @return Pointer to Snapshot.
     * @throw HolderHasWrongType, NullPointerException
     */
    Snapshot* to_snapshot(Holder* holder);

    /**
     * @copydoc to_snapshot(Holder*)
     */
    const Snapshot* to_snapshot(const Holder* holder);

}

#endif

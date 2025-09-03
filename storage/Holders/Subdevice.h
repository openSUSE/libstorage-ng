/*
 * Copyright (c) [2014-2015] Novell, Inc.
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


#ifndef STORAGE_SUBDEVICE_H
#define STORAGE_SUBDEVICE_H


#include "storage/Holders/Holder.h"


namespace storage
{

    /**
     * Generic holder from one device to a subdevice, e.g. from a PartitionTable to a
     * Partition or from a LvmVg to a LvmLv.
     *
     * @see Holder
     */
    class Subdevice : public Holder
    {
    public:

	/**
	 * Create a holder of type Subdevice. Usually this function is not called
	 * directly. Functions like PartitionTable::create_partition() or
	 * LvmVg::add_lvm_pv() call it.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static Subdevice* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static Subdevice* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual Subdevice* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	Subdevice(Impl* impl);
	ST_NO_SWIG Subdevice(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to a Subdevice.
     *
     * @throw NullPointerException
     */
    bool is_subdevice(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to Subdevice.
     *
     * @return Pointer to Subdevice.
     * @throw HolderHasWrongType, NullPointerException
     */
    Subdevice* to_subdevice(Holder* holder);

    /**
     * @copydoc to_user(Holder*)
     */
    const Subdevice* to_subdevice(const Holder* holder);

}

#endif

/*
 * Copyright (c) [2017-2020] SUSE LLC
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


#ifndef STORAGE_MD_SUBDEVICE_H
#define STORAGE_MD_SUBDEVICE_H


#include <string>

#include "storage/Holders/Subdevice.h"


namespace storage
{

    /**
     * Holder from a MdContainer to a MdMember.
     *
     * @see Subdevice, Holder
     */
    class MdSubdevice : public Subdevice
    {
    public:

	/**
	 * Create a holder of type MdSubdevice. Usually this function is not called
	 * directly. It is called during probing.
	 *
	 * @see Holder::create(Devicegraph*, const Device*, const Device*)
	 *
	 * @throw HolderAlreadyExists
	 */
	static MdSubdevice* create(Devicegraph* devicegraph, const Device* source, const Device* target);

	static MdSubdevice* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual MdSubdevice* clone() const override;
	virtual std::unique_ptr<Holder> clone_v2() const override;

	/**
	 * The "member" string of the MdMember in the MdContainer.
	 */
	const std::string& get_member() const;

	void set_member(const std::string& member);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	MdSubdevice(Impl* impl);
	ST_NO_SWIG MdSubdevice(std::unique_ptr<Holder::Impl>&& impl);

    };


    /**
     * Checks whether holder points to an MdSubdevice.
     *
     * @throw NullPointerException
     */
    bool is_md_subdevice(const Holder* holder);

    /**
     * Converts pointer to Holder to pointer to MdSubdevice.
     *
     * @return Pointer to MdSubdevice.
     * @throw HolderHasWrongType, NullPointerException
     */
    MdSubdevice* to_md_subdevice(Holder* holder);

    /**
     * @copydoc to_md_subdevice(Holder*)
     */
    const MdSubdevice* to_md_subdevice(const Holder* holder);

}

#endif

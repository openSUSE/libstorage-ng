/*
 * Copyright (c) 2017 SUSE LLC
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
     */
    class MdSubdevice : public Subdevice
    {
    public:

	static MdSubdevice* create(Devicegraph* devicegraph, const Device* source, const Device* target);
	static MdSubdevice* load(Devicegraph* devicegraph, const xmlNode* node);

	virtual MdSubdevice* clone() const override;

	/**
	 * The "member" string of the MdMember in the MdContainer.
	 */
	const std::string& get_member() const;

	void set_member(const std::string& member);

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

    protected:

	MdSubdevice(Impl* impl);

    };


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

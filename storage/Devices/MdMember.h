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


#ifndef STORAGE_MD_MEMBER_H
#define STORAGE_MD_MEMBER_H


#include "storage/Devices/Md.h"


namespace storage
{

    class MdContainer;


    /**
     * A MD member
     */
    class MdMember : public Md
    {
    public:

	/**
	 * @throw Exception
	 */
	static MdMember* create(Devicegraph* devicegraph, const std::string& name);

	static MdMember* load(Devicegraph* devicegraph, const xmlNode* node);

	/**
	 * @throw Exception
	 */
	MdContainer* get_md_container();

	/**
	 * @throw Exception
	 */
	const MdContainer* get_md_container() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual MdMember* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	MdMember(Impl* impl);
	ST_NO_SWIG MdMember(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to a MdMember.
     *
     * @throw NullPointerException
     */
    bool is_md_member(const Device* device);

    /**
     * Converts pointer to Device to pointer to MdMember.
     *
     * @return Pointer to MdMember.
     * @throw DeviceHasWrongType, NullPointerException
     */
    MdMember* to_md_member(Device* device);

    /**
     * @copydoc to_md_member(Device*)
     */
    const MdMember* to_md_member(const Device* device);

}

#endif

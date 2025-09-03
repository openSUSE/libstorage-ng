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


#ifndef STORAGE_MD_CONTAINER_H
#define STORAGE_MD_CONTAINER_H


#include "storage/Devices/Md.h"


namespace storage
{

    class MdMember;


    /**
     * A MD Container
     */
    class MdContainer : public Md
    {
    public:

	/**
	 * @throw Exception
	 */
	static MdContainer* create(Devicegraph* devicegraph, const std::string& name);

	static MdContainer* load(Devicegraph* devicegraph, const xmlNode* node);

	std::vector<MdMember*> get_md_members();
	std::vector<const MdMember*> get_md_members() const;

    public:

	class Impl;

	Impl& get_impl();
	const Impl& get_impl() const;

	virtual MdContainer* clone() const override;
	virtual std::unique_ptr<Device> clone_v2() const override;

	MdContainer(Impl* impl);
	ST_NO_SWIG MdContainer(std::unique_ptr<Device::Impl>&& impl);

    };


    /**
     * Checks whether device points to an MdContainer.
     *
     * @throw NullPointerException
     */
    bool is_md_container(const Device* device);

    /**
     * Converts pointer to Device to pointer to MdContainer.
     *
     * @return Pointer to MdContainer.
     * @throw DeviceHasWrongType, NullPointerException
     */
    MdContainer* to_md_container(Device* device);

    /**
     * @copydoc to_md_container(Device*)
     */
    const MdContainer* to_md_container(const Device* device);

}

#endif

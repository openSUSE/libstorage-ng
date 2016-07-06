/*
 * Copyright (c) 2016 SUSE LLC
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


#include "storage/Devices/EncryptionImpl.h"
#include "storage/Devicegraph.h"
#include "storage/Action.h"


namespace storage
{

    using namespace std;


    Encryption*
    Encryption::create(Devicegraph* devicegraph, const string& name)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    Encryption*
    Encryption::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	Encryption* ret = new Encryption(new Encryption::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    Encryption::Encryption(Impl* impl)
	: BlkDevice(impl)
    {
    }


    Encryption*
    Encryption::clone() const
    {
	return new Encryption(get_impl().clone());
    }


    Encryption::Impl&
    Encryption::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const Encryption::Impl&
    Encryption::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    const BlkDevice*
    Encryption::get_blk_device() const
    {
	return get_impl().get_blk_device();
    }


    bool
    is_encryption(const Device* device)
    {
	return is_device_of_type<const Encryption>(device);
    }


    Encryption*
    to_encryption(Device* device)
    {
	return to_device_of_type<Encryption>(device);
    }


    const Encryption*
    to_encryption(const Device* device)
    {
	return to_device_of_type<const Encryption>(device);
    }

}

/*
 * Copyright (c) 2019 SUSE LLC
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


#include "storage/Devices/PlainEncryptionImpl.h"
#include "storage/Devicegraph.h"


namespace storage
{

    using namespace std;


    PlainEncryption*
    PlainEncryption::create(Devicegraph* devicegraph, const string& name)
    {
	PlainEncryption* ret = new PlainEncryption(new PlainEncryption::Impl(name));
	ret->Device::create(devicegraph);
	return ret;
    }


    PlainEncryption*
    PlainEncryption::load(Devicegraph* devicegraph, const xmlNode* node)
    {
	PlainEncryption* ret = new PlainEncryption(new PlainEncryption::Impl(node));
	ret->Device::load(devicegraph);
	return ret;
    }


    PlainEncryption::PlainEncryption(Impl* impl)
	: Encryption(impl)
    {
    }


    PlainEncryption*
    PlainEncryption::clone() const
    {
	return new PlainEncryption(get_impl().clone());
    }


    PlainEncryption::Impl&
    PlainEncryption::get_impl()
    {
	return dynamic_cast<Impl&>(Device::get_impl());
    }


    const PlainEncryption::Impl&
    PlainEncryption::get_impl() const
    {
	return dynamic_cast<const Impl&>(Device::get_impl());
    }


    vector<PlainEncryption*>
    PlainEncryption::get_all(Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<PlainEncryption>();
    }


    vector<const PlainEncryption*>
    PlainEncryption::get_all(const Devicegraph* devicegraph)
    {
	return devicegraph->get_impl().get_devices_of_type<const PlainEncryption>();
    }


    bool
    is_plain_encryption(const Device* device)
    {
	return is_device_of_type<const PlainEncryption>(device);
    }


    PlainEncryption*
    to_plain_encryption(Device* device)
    {
	return to_device_of_type<PlainEncryption>(device);
    }


    const PlainEncryption*
    to_plain_encryption(const Device* device)
    {
	return to_device_of_type<const PlainEncryption>(device);
    }

}
